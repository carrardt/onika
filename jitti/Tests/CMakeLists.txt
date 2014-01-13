include_directories("..")
add_executable(jittitest main.cpp)
target_link_libraries(jittitest jitti)

add_dependencies(jittitest generate_jit_sources)
