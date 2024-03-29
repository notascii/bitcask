Basic usage:
```bash
mkdir build
cd build

cmake ..
# or
cmake -DCMAKE_BUILD_TYPE=Debug .. # for debug build
cmake -DBUILD_WITH_DOCS=ON .. # to build the documentation

cmake --build .

# Run the executable
./bitcask-cli <db_path> # type 'help' to see the available commands

# Run the tests
ctest

# Make the documentation
make docs

# Static analysis
cppcheck --enable=all --supress=missingIncludeSystem *.cpp

# Memory leak detection
valgrind --leak-check=full ./bitcask-cli <db_path>

# Formatting
clang-format --style=Microsoft -i *.cpp *.h
```