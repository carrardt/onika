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

set(UGRID_SMESH_COMPRESS_NAME "ugridsmeshcompress.cxx")
set(UGRID_SMESH_COMPRESS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/jit/ugridsmeshcompress.cxx")
set(UGRID_SMESH_COMPRESS_JIT "${CMAKE_CURRENT_BINARY_DIR}/ugridsmeshcompress.cxx")

#${CMAKE_CXX_COMPILER}
add_custom_target(preprocess_jit_sources
	${CLANG_COMPILER} -xc++ -std=c++11 -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/../include -E "${UGRID_SMESH_COMPRESS_SRC}" -o "${UGRID_SMESH_COMPRESS_JIT}"
	DEPENDS ${UGRID_SMESH_COMPRESS_SRC})

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "../jitti" )
add_definitions("-DUGRID_SMESH_COMPRESS_JIT=\"${UGRID_SMESH_COMPRESS_JIT}\"")
add_library(vtkUGridSMeshCompress SHARED vtkUGridSMeshCompress.cxx vtkUGridDescription.cxx)
add_dependencies(vtkUGridSMeshCompress preprocess_jit_sources)
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

