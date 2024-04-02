# Bitcask
A simple key-value store based on the Bitcask storage format implemented in C++.

A Command Line Interface (CLI) executable (*bitcask-cli*) is provided to interact with the
key-value store library (*bitcask*).

## Basic Usage
```bash
# Clone the repository
git clone git@github.com:notascii/bitcask.git
cd bitcask

# Initialize the submodules
git submodule init
git submodule update

mkdir build
cd build

# Configure the build
cmake ..
# or a mix of the following options
# cmake -DCMAKE_BUILD_TYPE=Debug .. # for debug build
# cmake -DBUILD_WITH_DOCS=ON .. # to build the documentation

# Build the project
cmake --build .

make docs # to build the documentation (optional if BUILD_WITH_DOCS=ON)
# then open the documentation (build/docs/html/index.html) in a browser

# Run the executable
./bitcask-cli <db_path> # then type 'help' to see the available commands
# Example:
# mkdir /tmp/bitcask_db/ && ./bitcask-cli /tmp/bitcask_db/
```

## Development
```bash
# Run the tests
ctest --output-on-failure

# Static analysis
cppcheck --enable=all --supress=missingIncludeSystem *.cpp

# Memory leak detection
valgrind --leak-check=full ./bitcask-cli <db_path>

# Formatting (style used in the project is Microsoft)
clang-format --style=Microsoft -i *.cpp *.h
```