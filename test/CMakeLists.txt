add_subdirectory(gtest)

add_library(test_utilities test_utilities.cpp)
target_link_libraries(test_utilities hpcgeo)
target_include_directories(test_utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(TESTS
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

add_executable(vertex_dedup_test vertex_dedup_test.cpp)
target_link_libraries(vertex_dedup_test test_utilities hpcgeo)
