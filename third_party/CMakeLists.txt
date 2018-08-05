add_subdirectory(glfw-3.2.1)
add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.2.1/deps/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.2.1/deps)

