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

add_subdirectory(Tests)

IF(${BUILD_PARAVIEW_PLUGIN}) 
	 # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
	ADD_PARAVIEW_PLUGIN(
	   Onika
	   "1.0"
	   SERVER_MANAGER_XML Onika.xml
	   SERVER_MANAGER_SOURCES vtkOnikaWriter.cxx
	   )
ENDIF(${BUILD_PARAVIEW_PLUGIN})

