set(LLVM_LINK_COMPONENTS
  Option
  Support
  )

add_clang_executable(jitti jitti.cpp)

target_link_libraries(jitti
  clangDriver
  clangCodeGen
  LLVMExecutionEngine
  LLVMJIT
  )

install(TARGETS jitti RUNTIME DESTINATION bin)
