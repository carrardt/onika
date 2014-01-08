
# to be generalized with more sources
add_custom_command(OUTPUT simplecpp11pp.cxx COMMAND ${CMAKE_CXX_COMPILER} -std=c++11 -E ${CMAKE_CURRENT_SOURCE_DIR}/simplecpp11.cxx -o simplecpp11pp.cxx)
add_custom_target(generate_test_sources DEPENDS simplecpp11pp.cxx)
