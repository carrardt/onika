cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(Onika)

include_directories("include")
set(CMAKE_CXX_FLAGS "-std=c++11")

add_subdirectory(Tests)
add_subdirectory(jitti)
add_subdirectory(PVCompressionPlugin)
add_subdirectory(jitsrc)
