 # Locate ParaView build and then import CMake configuration, 
 # macros etc. from it.

set(BUILD_PARAVIEW_PLUGIN true CACHE BOOL "Build Paraview Plugin")

if(${BUILD_PARAVIEW_PLUGIN})
	FIND_PACKAGE(ParaView REQUIRED)
	INCLUDE(${PARAVIEW_USE_FILE}) 
else(${BUILD_PARAVIEW_PLUGIN})
	FIND_PACKAGE(VTK REQUIRED)
	INCLUDE(${VTK_USE_FILE})
endif(${BUILD_PARAVIEW_PLUGIN})

add_executable(genugriddesc genugriddesc.cxx vtkUGridDescription.cxx)
target_link_libraries(genugriddesc ${VTK_LIBRARIES})

add_custom_target(generate_test_data_desc
	genugriddesc "${TEST_UGRID_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/testdata.h" " \#define UGRID_DESC "
	DEPENDS genugriddesc)

add_subdirectory(Tests)

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "../jitti" )
add_definitions(${ugridsmeshcompress_CFLAGS})
add_library(vtkUGridSMeshCompress SHARED vtkUGridSMeshCompress.cxx vtkUGridDescription.cxx)
add_dependencies(vtkUGridSMeshCompress generate_jit_sources)
target_link_libraries(vtkUGridSMeshCompress jitti ${VTK_LIBRARIES})

IF(${BUILD_PARAVIEW_PLUGIN}) 
	 # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
	ADD_PARAVIEW_PLUGIN(
	   Onika
	   "1.0"
	   SERVER_MANAGER_XML Onika.xml
	   SERVER_MANAGER_SOURCES vtkOnikaWriter.cxx
	   )
ENDIF(${BUILD_PARAVIEW_PLUGIN})

