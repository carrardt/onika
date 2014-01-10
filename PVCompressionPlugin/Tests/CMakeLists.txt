set(TEST_UGRID_FILE "${Onika_SOURCE_DIR}/data/diskoutref.vtu" CACHE STRING "Unstructured grid test file")

include_directories(".." "${CMAKE_CURRENT_BINARY_DIR}/..")
 
add_executable(ugrid_tetra_TEST ugrid_tetra.cxx ../vtkUGridDescription.cxx)
add_dependencies(ugrid_tetra_TEST generate_test_data_desc)
target_link_libraries(ugrid_tetra_TEST ${VTK_LIBRARIES})

add_executable(compress_ugrid_tetra_TEST compress_ugrid_tetra.cxx ../vtkUGridDescription.cxx)
target_link_libraries(compress_ugrid_tetra_TEST vtkUGridSMeshCompress ${VTK_LIBRARIES})
