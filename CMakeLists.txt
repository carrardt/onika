cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(Onika)

set(ONIKA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${ONIKA_INCLUDE_DIR}")
set(CMAKE_CXX_FLAGS "-std=c++11")

add_subdirectory(Tests)
add_subdirectory(jitti)
add_subdirectory(jitsrc)
add_subdirectory(PVCompressionPlugin)
