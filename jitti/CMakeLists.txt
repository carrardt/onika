set(LLVM_LINK_COMPONENTS
  Option
  Support
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_clang_executable(jitti jitti.cpp)

target_link_libraries(jitti
  clangDriver
  clangCodeGen
  LLVMExecutionEngine
  LLVMJIT
  )

install(TARGETS jitti RUNTIME DESTINATION bin)
