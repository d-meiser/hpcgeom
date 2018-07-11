cmake_minimum_required(VERSION 3.0.0)
project(HPCGeom)

enable_testing()
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
add_subdirectory(test)
