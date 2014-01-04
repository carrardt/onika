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
  clangTooling
  clangBasic
  clangRewriteFrontend
  )

install(TARGETS jitti
  RUNTIME DESTINATION bin)
