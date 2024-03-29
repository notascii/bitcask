#include <gtest/gtest.h>
#include <fstream> // std::ofstream

#include "../include/bitcask_handle.hpp"


TEST(BitcaskHandleConstructor, ValidPath)
{
    std::string db_path = "/tmp/test_bitcask/data";

    // creating a temporary directory for testing using filesystem
    if (std::filesystem::exists(db_path)) // if the directory already exists, remove its contents
    {
        std::filesystem::remove_all(db_path);
    }
    if (!std::filesystem::create_directories(db_path))
    {
        std::cerr << "Failed to create directory for testing" << std::endl;
    }

    ASSERT_TRUE(std::filesystem::exists(db_path));
    ASSERT_TRUE(std::filesystem::is_directory(db_path));
    ASSERT_TRUE(std::filesystem::is_empty(db_path));

    bitcask::BitcaskHandle handle(db_path);
    EXPECT_EQ(handle.get_db_path(), db_path);
}


TEST(BitcaskHandleConstructor, NonExistentPath)
{
    EXPECT_THROW(bitcask::BitcaskHandle handle("/home/non/existent"), std::invalid_argument);
}


TEST(BitcaskHandleConstructor, NonEmptyDirectory)
{
    std::string db_path = "/tmp/test_bitcask/data";
    // creating a temporary directory for testing using filesystem
    if (std::filesystem::exists(db_path)) // if the directory already exists, remove its contents
    {
        std::filesystem::remove_all(db_path);
    }
    if (!std::filesystem::create_directories(db_path))
    {
        std::cerr << "Failed to create directory for testing" << std::endl;
    }

    // Create a file inside the directory
    std::ofstream file(db_path + "/datafile1.txt");
    file << "This is a test file";
    file.close();

    ASSERT_TRUE(std::filesystem::exists(db_path));
    ASSERT_TRUE(std::filesystem::is_directory(db_path));
    ASSERT_FALSE(std::filesystem::is_empty(db_path));

    // Expect an exception to be thrown when creating a BitcaskHandle object
    // with a non-empty directory
    EXPECT_THROW(bitcask::BitcaskHandle handle(db_path), std::invalid_argument);
}
