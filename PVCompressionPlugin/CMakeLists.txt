 # Locate ParaView build and then import CMake configuration, 
 # macros etc. from it.
 
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})
 
add_subdirectory(Tests)
 
 # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   Onika
   "1.0"
   SERVER_MANAGER_XML Onika.xml
   SOURCES meshcompress.cxx
   SERVER_MANAGER_SOURCES vtkOnikaWriter.cxx
 )

