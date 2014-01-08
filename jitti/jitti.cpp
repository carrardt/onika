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
		std::map<std::string,FunctionPriv> m_entryPoints;
		llvm::ExecutionEngine* m_executionEngine;
	};

	Module::Module( const Module& m )
	{
		priv = new ModulePriv;
		*priv = *(m.priv);
	}
	Module::Module( Module&& m )
	{
		priv = m.priv;
		m.priv = 0;
	}
	Module::Module( llvm::Module* mod )
	{
		priv = new ModulePriv;
		priv->m_module = mod;
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

	Function Module::getFunction(const char* name)
	{
		auto it = priv->m_entryPoints.find(name);
		if( it != priv->m_entryPoints.end() )
		{
			return Function( & it->second );
		}
		llvm::Function* F = priv->m_module->getFunction(name);
		if ( F == 0 )
		{
		   	llvm::errs() <<"function '"<<name<< "'  not found in module.\n";
		}
		FunctionPriv fdata = { priv->m_executionEngine , F };
		priv->m_entryPoints[name] = fdata; 
		return Function( & priv->m_entryPoints[name] );
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
		inline Module compileFile(const char* filePath, const char* args_in)
		{
			  const char * MyArgs[256] = {"-xc++","-std=c++11","-c",0};
			  int MyArgc = 0; while(MyArgs[MyArgc]!=0) ++MyArgc;

			  char* args = strdup(args_in);
			  while( *args != '\0' && MyArgc<255 )
			  {
				  while( *args != '\0' && std::isspace(*args) ) { *args='\0'; ++args; }
				  if( *args != '\0' ) MyArgs[MyArgc++] = args;
				  while( *args != '\0' && ! std::isspace(*args) ) { ++args; }
			  }
			  MyArgs[MyArgc++] = filePath;
			  MyArgs[MyArgc] = 0;

			  for(int i=0;i<MyArgc;i++) llvm::errs() << MyArgs[i] << "\n";

			  clang::ArrayRef<const char*> argsRef(MyArgs,MyArgc);
			  compilation = driver->BuildCompilation(argsRef);
			  if( compilation == 0 )
			  {
				  llvm::errs() << "Unable to build compilation";
				  return 0;
			  }
//			  free(args);


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

			  return Module( codeGenAction->takeModule() );
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
	
	Module Compiler::createModuleFromFile(const char* filePath,const char* opt_args)
	{
		return CompilerImp::instance()->compileFile(filePath,opt_args);
	}
}

