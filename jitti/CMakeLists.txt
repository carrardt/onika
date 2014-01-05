set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  asmparser
  bitreader
  support
  mc
  )

add_clang_executable(jitti
  jitti.cpp
  )

target_link_libraries(jitti
  clangCodeGen
  LLVMExecutionEngine
  LLVMJIT
  )

#set(LLVM_VERSION "3.4" CACHE STRING "LLVM/clang Version to use")
#set(LLVM_LIB_DIR "/usr/lib/llvm-${LLVM_VERSION}/lib")
#set(LLVM_INC_DIR "/usr/include/llvm-${LLVM_VERSION}")
#add_executable(jitti jitti.cpp)
#target_link_libraries(jitti -L${LLVM_LIB_DIR} -Wl,-rpath,${LLVM_LIB_DIR} clang LLVM-${LLVM_VERSION})
#target_include_directories(jitti PRIVATE "${LLVM_INC_DIR}" PRIVATE "." )

install(TARGETS jitti RUNTIME DESTINATION bin)
