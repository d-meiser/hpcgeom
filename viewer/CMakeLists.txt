add_executable(viewer viewer.c)
target_compile_options(viewer  PRIVATE -Wall -Wextra -Werror)
target_link_libraries(viewer hpcgeo glfw glad m)
