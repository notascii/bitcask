# add_executable(test_bitcask_handle test_bitcask_handle.cpp)

# target_link_libraries(test_bitcask_handle GTest::gtest_main)

add_executable(test_bitcask_handle test_bitcask_handle.cpp)
target_link_libraries(test_bitcask_handle GTest::gtest_main bitcask)
# add_test(NAME test_bitcask_handle COMMAND test_bitcask_handle)

include(CTest)
include(GoogleTest)
gtest_discover_tests(test_bitcask_handle)

add_subdirectory(benchmark)
