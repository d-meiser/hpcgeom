add_library(gtest src/gtest-all.cc)
target_include_directories(gtest
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads)
target_link_libraries(gtest INTERFACE Threads::Threads)

add_library(gtest_main src/gtest_main.cc)
target_include_directories(gtest_main
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	)
target_link_libraries(gtest_main INTERFACE gtest)

