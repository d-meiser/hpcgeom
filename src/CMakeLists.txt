add_library(hpcgeo
	vertex_array.c
	spatial_hash.c
	)
target_include_directories(hpcgeo
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}
	)
target_compile_options(hpcgeo PRIVATE -Wall -Wextra -Werror)
