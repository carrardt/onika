include_directories("..")
add_executable(data_array_wrapper_TEST data_array_wrapper.cxx)
target_link_libraries(data_array_wrapper_TEST ${VTK_LIBRARIES})