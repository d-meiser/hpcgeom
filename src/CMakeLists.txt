add_library(hpcgeo vertex_array.cpp)
target_include_directories(hpcgeo
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
set_property(TARGET hpcgeo PROPERTY CXX_STANDARD 11)
