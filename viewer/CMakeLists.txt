add_executable(viewer viewer.c camera.c)
target_compile_options(viewer  PRIVATE -Wall -Wextra -Werror)
target_link_libraries(viewer hpcgeo glfw glad m)
target_include_directories(viewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
