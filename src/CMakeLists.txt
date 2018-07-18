add_library(hpcgeo
	basic_types.c
	hashed_octree.c
	qsort.cpp
	spatial_hash.c
	transformation.c
	vertex_array.c
	vertex_set.c
	)
target_include_directories(hpcgeo
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	)
target_compile_options(hpcgeo PRIVATE -Wall -Wextra -Werror)
