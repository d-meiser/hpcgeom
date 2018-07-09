add_subdirectory(gtest)

set(TESTS
	vertex_array
	)
foreach(t ${TESTS})
	add_executable(${t}_test ${t}_test.cpp)
	target_link_libraries(${t}_test hpcgeo gtest_main)
	add_test(${t} ${t}_test)
endforeach()
