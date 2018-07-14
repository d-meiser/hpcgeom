add_library(hpcgeo
	hashed_octree.c
	spatial_hash.c
	vertex_array.c
	qsort.cpp
	)
target_include_directories(hpcgeo
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)
target_compile_options(hpcgeo PRIVATE -Wall -Wextra -Werror)
