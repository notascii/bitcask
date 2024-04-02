function(add_test_file test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} GTest::gtest_main bitcask)
    gtest_discover_tests(${test_name})
endfunction()

include(GoogleTest)

file(GLOB TEST_FILES "test_*.cpp")

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE}.cpp NAME_WE)
    add_test_file(${TEST_NAME})
endforeach()

# add_test_file(test_bitcask_handle)
# add_test_file(test_keydir)


# --- benchmark ---
add_subdirectory(benchmark)
