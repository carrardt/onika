cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(Onika)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("include")
set(CMAKE_CXX_FLAGS "-std=c++11")

add_subdirectory(Tests)
#add_subdirectory(jitti)
add_subdirectory(PVCompressionPlugin)

