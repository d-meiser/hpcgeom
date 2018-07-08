add_subdirectory(gtest)
add_executable(gtest_test gtest_test.cpp)
target_link_libraries(gtest_test gtest_main)
add_test(gtest_test gtest_test)
