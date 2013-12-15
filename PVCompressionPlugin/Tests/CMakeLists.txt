include_directories("..")
add_executable(data_array_wrapper_TEST data_array_wrapper.cxx)
target_link_libraries(data_array_wrapper_TEST ${VTK_LIBRARIES})

add_executable(ugrid_tetra_TEST ugrid_tetra.cxx)
target_link_libraries(ugrid_tetra_TEST ${VTK_LIBRARIES})
