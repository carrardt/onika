 # Locate ParaView build and then import CMake configuration, 
 # macros etc. from it.
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})
 
 # Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
 ADD_PARAVIEW_PLUGIN(
   Onika                 #<--Name for the plugin
  "1.0"                        #<--Version string
   SERVER_MANAGER_XML Onika.xml  #<-- server manager xml
   SOURCES meshcompress.cxx
   SERVER_MANAGER_SOURCES vtkOnikaWriter.cxx #<-- source files for the new classes
 )

