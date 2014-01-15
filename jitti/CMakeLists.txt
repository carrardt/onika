set(LLVM_INC_DIR "/usr/local/include" CACHE STRING "LLVM include directory")
set(CLANG_INC_DIR "/usr/local/include/clang" CACHE STRING "LLVM include directory")
set(LLVM_LIB_DIR "/usr/local/lib" CACHE STRING "LLVM lib directory")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

include_directories("${LLVM_INC_DIR}" "${CLANG_INC_DIR}")
add_library(jitti SHARED jitti.cpp)

link_directories("${LLVM_LIB_DIR}")
target_link_libraries(jitti
	clangDriver
	clangCodeGen
	LLVMExecutionEngine
	LLVMJIT
	LLVMBitWriter
	LLVMIRReader
	LLVMInstrumentation
	LLVMipo
	LLVMLinker
	LLVMVectorize
	clangFrontend
	clangDriver
	LLVMOption
	clangParse
	clangSerialization
	LLVMBitReader
	clangSema
	LLVMAArch64AsmParser
	LLVMAArch64Disassembler
	LLVMARMCodeGen
	LLVMARMAsmParser
	LLVMARMDisassembler
	LLVMCppBackendCodeGen
	LLVMHexagonCodeGen
	LLVMMipsCodeGen
	LLVMMipsAsmParser
	LLVMMipsDisassembler
	LLVMMSP430CodeGen
	LLVMNVPTXCodeGen
	LLVMPowerPCCodeGen
	LLVMPowerPCAsmParser
	LLVMR600CodeGen
	LLVMSparcCodeGen
	LLVMSystemZCodeGen
	LLVMSystemZAsmParser
	LLVMSystemZDisassembler
	LLVMX86CodeGen
	LLVMX86AsmParser
	LLVMX86Disassembler
	LLVMXCoreCodeGen
	LLVMXCoreDisassembler
	LLVMAsmParser
	LLVMAArch64CodeGen
	LLVMARMDesc
	LLVMCppBackendInfo
	LLVMHexagonAsmPrinter
	LLVMMipsDesc
	LLVMMSP430Desc
	LLVMNVPTXDesc
	LLVMPowerPCDesc
	LLVMR600Desc
	LLVMSparcDesc
	LLVMSystemZDesc
	LLVMX86Desc
	LLVMXCoreDesc
	LLVMAArch64Desc
	LLVMAsmPrinter
	LLVMSelectionDAG
	LLVMARMAsmPrinter
	LLVMARMInfo
	LLVMHexagonDesc
	LLVMMipsAsmPrinter
	LLVMMipsInfo
	LLVMMSP430AsmPrinter
	LLVMMSP430Info
	LLVMNVPTXAsmPrinter
	LLVMNVPTXInfo
	LLVMPowerPCAsmPrinter
	LLVMPowerPCInfo
	LLVMR600AsmPrinter
	LLVMR600Info
	LLVMSparcInfo
	LLVMSystemZAsmPrinter
	LLVMSystemZInfo
	LLVMX86AsmPrinter
	LLVMX86Info
	LLVMXCoreAsmPrinter
	LLVMXCoreInfo
	LLVMAArch64AsmPrinter
	LLVMAArch64Info
	LLVMMCParser
	LLVMHexagonInfo
	LLVMX86Utils
	LLVMAArch64Utils
	clangEdit
	clangAnalysis
	clangAST
	clangLex
	clangBasic
	LLVMCodeGen
	LLVMObjCARCOpts
	LLVMScalarOpts
	LLVMInstCombine
	LLVMTransformUtils
	LLVMipa
	LLVMAnalysis
	LLVMExecutionEngine
	LLVMTarget
	LLVMCore
	LLVMMC
	LLVMRuntimeDyld
	LLVMObject
	LLVMSupport
	rt
	dl
#	tinfo
	pthread
	z
)

add_subdirectory(Tests)

