add_subdirectory(gtest)

set(TESTS
	hashed_octree
	spatial_hash
	vertex_array
	vertex_set
	)
foreach(t ${TESTS})
	add_executable(${t}_test ${t}_test.cpp)
	target_link_libraries(${t}_test hpcgeo gtest_main)
	add_test(${t} ${t}_test)
endforeach()
