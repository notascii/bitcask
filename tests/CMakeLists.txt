include(GoogleTest)

# Set the test source files explicitly
set(TEST_SOURCES
    test_bitcask_handle.cpp
    test_keydir.cpp
    test_store.cpp
    # Add more test source files here if needed
)

# Iterate over each test source file and create corresponding test executables
foreach(TEST_SOURCE ${TEST_SOURCES})
    # Extract the test name from the source file
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Create the test executable for the current test
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    # Link the test executable with necessary libraries (e.g., GTest and your project library)
    target_link_libraries(${TEST_NAME} GTest::gtest_main bitcask)

    # Register the test with CTest (optional but recommended for test discovery)
    gtest_discover_tests(${TEST_NAME})
endforeach()

#function(add_test_file test_name)
#    add_executable(${test_name} ${test_name}.cpp)
#    target_link_libraries(${test_name} GTest::gtest_main bitcask)
#    gtest_discover_tests(${test_name})
#endfunction()
#
#include(GoogleTest)
#
#file(GLOB TEST_FILES "test_*.cpp")
#
#foreach(TEST_FILE ${TEST_FILES})
#    get_filename_component(TEST_NAME ${TEST_FILE}.cpp NAME_WE)
#    add_test_file(${TEST_NAME})
#endforeach()

# add_test_file(test_bitcask_handle)
# add_test_file(test_keydir)


# --- benchmark ---
#add_subdirectory(benchmark)
