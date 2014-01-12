//===--- tools/clang-check/ClangCheck.cpp - Clang check tool --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements a clang-check tool that runs clang based on the info
//  stored in a compilation database.
//
//  This tool uses the Clang Tooling infrastructure, see
//    http://clang.llvm.org/docs/HowToSetupToolingForLLVM.html
//  for details on setting it up with LLVM source tree.
//
//===----------------------------------------------------------------------===//
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "jitti.h"

#include <cctype>

namespace jitti
{
	struct FunctionPriv
	{
		llvm::ExecutionEngine* m_executionEngine;
		llvm::Function * m_entryPoint;
		std::vector<llvm::GenericValue> m_args;
		llvm::GenericValue m_ret;
	};

	Function::Function( llvm::ExecutionEngine* ee, llvm::Function* f )
	{
		priv = new FunctionPriv;
		priv->m_executionEngine = ee;
		priv->m_entryPoint = f;
	}

	Function::~Function()
	{
		if(priv!=0) { delete priv; }
	}

	void Function::resetCallArgs() { priv->m_args.clear(); }
	void Function::pushArgPtr(void * ptr) { llvm::GenericValue val; val.PointerVal=ptr; priv->m_args.push_back(val); } 
	void Function::pushArgInt(const int & x)  { llvm::GenericValue val; val.IntVal=llvm::APInt(sizeof(x)*8,x,true); priv->m_args.push_back(val); } 
	void Function::pushArgUInt(const unsigned int & x) { llvm::GenericValue val; val.IntVal=llvm::APInt(sizeof(x)*8,x,true); priv->m_args.push_back(val); }
	void Function::pushArgLong(const long long int & x) { llvm::GenericValue val; val.IntVal=llvm::APInt(sizeof(x)*8,x,true); priv->m_args.push_back(val); }
	void Function::pushArgULong(const unsigned long long int & x) { llvm::GenericValue val; val.IntVal=llvm::APInt(sizeof(x)*8,x,true); priv->m_args.push_back(val); }
	void Function::pushArgFloat(const float & x) { llvm::GenericValue val; val.FloatVal=x; priv->m_args.push_back(val);  }
	void Function::pushArgDouble(const double & x) { llvm::GenericValue val; val.DoubleVal=x; priv->m_args.push_back(val);  }

	void Function::call()
	{
		if( priv->m_executionEngine == 0 || priv->m_entryPoint == 0 )
		{
			llvm::errs() << "Execution error\n";
			return;
		}
		priv->m_ret = priv->m_executionEngine->runFunction( priv->m_entryPoint, priv->m_args );
	}

	void* Function::getReturnValueAsPtr() { return priv->m_ret.PointerVal; }
	uint64_t Function::getReturnValueAsULong() { return priv->m_ret.IntVal.getLimitedValue(); }
	float Function::getReturnValueAsFloat() { return priv->m_ret.FloatVal; }
	double Function::getReturnValueAsDouble() { return priv->m_ret.DoubleVal; }

	struct ModulePriv
	{
		llvm::Module* m_module;
		char** m_args;
		std::map<std::string,Function*> m_entryPoints;
		llvm::ExecutionEngine* m_executionEngine;
		inline ~ModulePriv()
		{
			for( auto it : m_entryPoints ) delete it.second;
			delete m_module;
			if( m_args != 0 )
			{
				if( *m_args != 0 ) delete [] *m_args;
				delete [] m_args;
			}
		}
	};

	Module::Module( llvm::Module* mod, char** args )
	{
		priv = new ModulePriv;
		priv->m_module = mod;
		priv->m_args = args;
		std::string Error;
		priv->m_executionEngine = llvm::ExecutionEngine::createJIT(priv->m_module, &Error);
		if ( ! priv->m_executionEngine )
		{
		  llvm::errs() << "unable to make execution engine: " << Error << "\n";
		}
	}
	Module::~Module()
	{
		if( priv != 0 ) delete priv;
	}

	Function& Module::getFunction(const char* name)
	{
		auto it = priv->m_entryPoints.find(name);
		if( it != priv->m_entryPoints.end() )
		{
			return * ( it->second );
		}
		llvm::Function* F = priv->m_module->getFunction(name);
		if ( F == 0 )
		{
		   	llvm::errs() <<"Function '"<<name<< "' not found in module.\n";
		}
		Function * mf = new Function( priv->m_executionEngine , F );
		priv->m_entryPoints[name] = mf;
		return * mf;
	}

	class CompilerImp
	{
	public:
		static inline CompilerImp* instance()
		{
			if( c_instance == 0 )
			{
				new CompilerImp();
			}
			return c_instance;
		}
	private:
		static CompilerImp* c_instance;
		inline CompilerImp()
		{
			assert(c_instance==0);
			c_instance = this;

			  llvm::InitializeNativeTarget();

			  // error printing mechanism
			  DiagOpts = new clang::DiagnosticOptions();
			  DiagClient = new clang::TextDiagnosticPrinter(llvm::errs(), DiagOpts);
			  DiagID = new clang::DiagnosticIDs();
			  Diags = new clang::DiagnosticsEngine(DiagID, DiagOpts, DiagClient);
			  driver = new clang::driver::Driver("jitti", llvm::sys::getProcessTriple(), "a.out", *Diags);
			  driver->setTitle("jitti");

			  compilation = 0;
			  codeGenAction = new clang::EmitLLVMOnlyAction();
		}

		inline ~CompilerImp()
		{
		  llvm::llvm_shutdown();
		}

	public:
		inline Module* compileFile(const char* filePath, const char* opt_args)
		{
			std::vector<std::string> argsv;
			argsv.push_back("-xc++");
			argsv.push_back("-std=c++11");
			argsv.push_back("-O3");
			argsv.push_back("-c");

			const char* p = opt_args;
			while( *p != '\0' )
			{
				while( *p!='\0' && std::isspace(*p) ) ++p;
				const char* s = p;
				while( *p!='\0' && !std::isspace(*p) ) ++p;
				argsv.push_back( std::string(s,p-s) );
			}
			argsv.push_back(filePath);

			  int na = argsv.size();
			  int total = 0;
			  for(int i=0;i<na;i++) total += argsv[i].length()+1;

			  char* argsa = new char [total];
			  char** argsp = new char* [ na+1 ];
			  for(int i=0;i<na;i++)
			  {
				  argsp[i] = argsa;
				  strcpy( argsa, argsv[i].c_str() );
				  argsa += argsv[i].length()+1;
			  }
			  argsp[na] = 0;

			  llvm::errs() << "clang";
			  for(int i=0;i<na;i++) llvm::errs() << " "<< argsp[i] ;
			  llvm::errs() <<"\n";

			  clang::ArrayRef<const char*> argsRef(argsp,na);
			  compilation = driver->BuildCompilation(argsRef);
			  if( compilation == 0 )
			  {
				  llvm::errs() << "Unable to build compilation";
				  return 0;
			  }

			  // We expect to get back exactly one command job, if we didn't something
			  // failed. Extract that job from the compilation.
			  const clang::driver::JobList &Jobs = compilation->getJobs();
			  if (Jobs.size() != 1 || !clang::isa<clang::driver::Command>(*Jobs.begin()))
			  {
				  Diags->Report(clang::diag::err_fe_expected_compiler_job);
			    return 0;
			  }

			  const clang::driver::Command *Cmd = clang::cast<clang::driver::Command>(*Jobs.begin());
			  if (llvm::StringRef(Cmd->getCreator().getName()) != "clang")
			  {
				  Diags->Report(clang::diag::err_fe_expected_clang_command);
			    return 0;
			  }

			  // Initialize a compiler invocation object from the clang (-cc1) arguments.
			  const clang::driver::ArgStringList &CCArgs = Cmd->getArguments();
			  clang::CompilerInvocation* CI = new clang::CompilerInvocation;
			  clang::CompilerInvocation::CreateFromArgs(*CI,
			                                     const_cast<const char **>(CCArgs.data()),
			                                     const_cast<const char **>(CCArgs.data()) +
			                                       CCArgs.size(),
			                                     * Diags );

//			  const std::vector< FrontendInputFile >& inputs = CI->getFrontendOpts().Inputs;
//			  for(int i=0;i<inputs.size();i++)
//			  {
//				  llvm::errs()<< inputs[i].getFile () << "\n";
//			  }

			  // Create a compiler instance to handle the actual work.
			  clang::CompilerInstance Clang;
			  Clang.setInvocation(CI);
			  CI=0;

			  // Create the compilers actual diagnostics engine.
			  Clang.createDiagnostics();
			  if (!Clang.hasDiagnostics())
			  {
				  llvm::errs()<< "no diagnostics\n";
			    return 0;
			  }

			  // Create and execute the frontend to generate an LLVM bitcode module.
			  if (!Clang.ExecuteAction(*codeGenAction))
			  {
				  llvm::errs()<< "compilation failed\n";
				  return 0;
			  }

			  return new Module( codeGenAction->takeModule() , argsp );
		}
	private:
		clang::driver::Driver* driver;
		clang::DiagnosticOptions* DiagOpts;
		clang::TextDiagnosticPrinter *DiagClient;
		clang::DiagnosticIDs* DiagID;
		clang::DiagnosticsEngine* Diags;
		clang::driver::Compilation* compilation;
		clang::CodeGenAction* codeGenAction;
	};

	CompilerImp* CompilerImp::c_instance = 0;
	
	Module* Compiler::createModuleFromFile(const char* filePath,const char* opt_args)
	{
		return CompilerImp::instance()->compileFile(filePath,opt_args);
	}
}

