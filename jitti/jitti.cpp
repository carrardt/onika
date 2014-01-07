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

namespace jitti
{
	class Module
	{
	public:

		inline Module( Module&& m )
			: m_module(m.m_module)
			, m_executionEngine(m.m_executionEngine)
		{
			m.m_module = 0;
			m.m_executionEngine = 0;
		}

		inline Module( llvm::Module* mod ) : m_module(mod)
		{
			  llvm::InitializeNativeTarget();

			  std::string Error;
			  m_executionEngine = llvm::ExecutionEngine::createJIT(m_module, &Error);
			  if (!m_executionEngine)
			  {
			    llvm::errs() << "unable to make execution engine: " << Error << "\n";
			  }
		}
		inline llvm::GenericValue call(const char* function)
		{
			  llvm::Function *EntryFn = m_module->getFunction(function);
			  if (!EntryFn) {
			    llvm::errs() <<"function '"<<function<< "'  not found in module.\n";
			    return llvm::GenericValue(0);
			  }

			  const char* cmesg = "Message passed from host code";
			  char* message = strdup(cmesg);
			  std::vector<llvm::GenericValue> Args;
			  Args.push_back( llvm::GenericValue( (void*)message ) );

			  return m_executionEngine->runFunction( EntryFn, Args );
		}
	private:
		llvm::Module* m_module;
		llvm::ExecutionEngine* m_executionEngine;
	};

	class Compiler
	{
	public:
		inline Compiler()
		{
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

		inline Module compileFile(const char* filePath)
		{
			  const char * MyArgs[] = {"-v","-xc++","-std=c++11","-c",filePath,0};
			  int MyArgc = 0; while(MyArgs[MyArgc]!=0) ++MyArgc;
			  clang::ArrayRef<const char*> args(MyArgs,MyArgc);
			  compilation = driver->BuildCompilation(args);
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
}

int main(int argc, const char **argv)
{
  std::string Path = argv[0];

  jitti::Compiler mycompiler;

  jitti::Module jitmodule = mycompiler.compileFile(argv[1]);
  jitmodule.call("justdoit");

  // Shutdown.
  llvm::llvm_shutdown();
}
