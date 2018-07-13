add_library(hpcgeo
	hashed_octree.c
	spatial_hash.c
	vertex_array.c
	)
target_include_directories(hpcgeo
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}
	)
target_compile_options(hpcgeo PRIVATE -Wall -Wextra -Werror)
