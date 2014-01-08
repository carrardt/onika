include_directories("..")
add_executable(jittitest main.cpp)
target_link_libraries(jittitest jitti)

add_subdirectory(data)

add_dependencies(jittitest generate_test_sources)
