set(TEST_UGRID_FILE "${Onika_SOURCE_DIR}/data/diskoutref.vtu" CACHE STRING "Unstructured grid test file")

include_directories(".." "${CMAKE_CURRENT_BINARY_DIR}")
 
add_executable(data_array_wrapper_TEST data_array_wrapper.cxx)
target_link_libraries(data_array_wrapper_TEST ${VTK_LIBRARIES})

add_custom_target(generate_test_data_desc
				  genugriddesc "${TEST_UGRID_FILE}" "testdata.h" " \#define UGRID_DESC "
				  DEPENDS genugriddesc)

add_executable(ugrid_tetra_TEST ugrid_tetra.cxx)
add_dependencies(ugrid_tetra_TEST generate_test_data_desc)
target_link_libraries(ugrid_tetra_TEST ${VTK_LIBRARIES})

add_executable(compress_ugrid_tetra_TEST compress_ugrid_tetra.cxx)
add_dependencies(compress_ugrid_tetra_TEST generate_test_data_desc)
target_link_libraries(compress_ugrid_tetra_TEST ${VTK_LIBRARIES})
