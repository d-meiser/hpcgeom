add_library(hpcgeo vertex_array.c)
target_include_directories(hpcgeo
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(hpcgeo PRIVATE -Wall -Wextra -Werror)
