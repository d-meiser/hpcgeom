add_subdirectory(gtest)

add_library(test_utilities test_utilities.cpp)
target_link_libraries(test_utilities hpcgeo)
target_include_directories(test_utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(test_utilities
	PROPERTIES
	CXX_VISIBILITY_PRESET default
	VISIBILITY_INLINES_HIDDEN 0
	)

set(TESTS
	edge_array
	hashed_bvh
	hashed_octree
	spatial_hash
	vertex_array
	vertex_set
	)
foreach(t ${TESTS})
	add_executable(${t}_test ${t}_test.cpp)
	target_link_libraries(${t}_test hpcgeo test_utilities gtest_main)
	add_test(${t} ${t}_test)
endforeach()

set(PERFORMANCE_TESTS
	transformation
	vertex_dedup
	)
foreach(t ${PERFORMANCE_TESTS})
	add_executable(${t}_test ${t}_test.cpp)
	target_link_libraries(${t}_test test_utilities hpcgeo)
endforeach()
