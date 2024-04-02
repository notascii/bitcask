cmake_minimum_required(VERSION 3.10)
project(bitcask VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Bitcask implementation in C++")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror")

# Set include directories
include_directories(include)

# Add sources
add_subdirectory(src)

# --- Creating shared library "bitcask" ---
add_library(bitcask STATIC ${SOURCES})
set_target_properties(bitcask PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(bitcask PROPERTIES PUBLIC_HEADER include/bitcask_handle.hpp)
target_include_directories(bitcask PRIVATE include)
# target_compile_options(bitcask PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
target_link_libraries(bitcask absl::status absl::statusor absl::strings)

# --- Creating main executable "bitcask-cli" ---
add_executable(bitcask-cli bitcask-cli.cpp ${SOURCES})
# Link bitcask-cli with bitcask library and abseil-cpp
target_link_libraries(bitcask-cli bitcask absl::status absl::statusor)
# Add -fanalyzer flag only to the bitcask-cli target
# target_compile_options(bitcask-cli PRIVATE -fanalyzer) # -> raises warnings from absl library


# --- Google Test ---
# Download and unpack googletest at configure time
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
# --- end Google Test ---


# --- Useless ? ---
# Set test files
# file(GLOB TEST_SOURCES
#     test/*.cpp
# )
# include(GoogleTest)  # --> this is included in tests/ CMakelists.txt
# -----------------


# Enable testing for current directory and below
enable_testing()

# add test subdirectory
add_subdirectory(tests)

# --- AbseilCpp ---
add_subdirectory(abseil-cpp)

# --- Generate Docs ---
option(BUILD_WITH_DOCS "Generate Docs" ON)
if(BUILD_WITH_DOCS)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/docs)
endif()
# --- end Generate Docs ---
