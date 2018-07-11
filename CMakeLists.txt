cmake_minimum_required(VERSION 3.0.0)
project(HPCGeom)

enable_testing()
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
add_subdirectory(test)

set (GEO_HAVE_FUNCTION_MULTI_DISPATH FALSE)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	if (CMAKE_C_COMPILER_VERSION VERSION_GREATER "6.0.0")
		set (GEO_HAVE_FUNCTION_MULTI_DISPATH TRUE)
	endif ()
endif ()
configure_file(geo_config.h.in ${PROJECT_BINARY_DIR}/geo_config.h)
