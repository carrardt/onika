set(TEST_UGRID_FILE "${Onika_SOURCE_DIR}/data/diskoutref.vtu" CACHE BOOL "Unstructured grid test file")

include_directories("..")
add_executable(data_array_wrapper_TEST data_array_wrapper.cxx)
target_link_libraries(data_array_wrapper_TEST ${VTK_LIBRARIES})

add_executable(genugriddesc genugriddesc.cxx)
target_link_libraries(genugriddesc ${VTK_LIBRARIES})
	
add_executable(ugrid_tetra_TEST ugrid_tetra.cxx)
target_link_libraries(ugrid_tetra_TEST ${VTK_LIBRARIES})

add_executable(compress_ugrid_tetra_TEST compress_ugrid_tetra.cxx ../meshcompress.cxx)
target_link_libraries(compress_ugrid_tetra_TEST ${VTK_LIBRARIES})
