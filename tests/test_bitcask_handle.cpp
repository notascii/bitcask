#include <gtest/gtest.h>
#include <fstream> // std::ofstream

#include "../include/bitcask_handle.hpp"


TEST(BitcaskHandleConstructor, ValidPath)
{
    // creating a temporary directory for testing using filesystem
    std::string db_path = "/tmp/test_bitcask/test_bitcask_handle/valid_path/";
    if (!std::filesystem::exists(db_path)) // if the directory already exists, remove its contents
    {
        if (!std::filesystem::create_directories(db_path))
        {
            std::cerr << "Failed to create directory for testing" << std::endl;
        }
    }

    ASSERT_TRUE(std::filesystem::exists(db_path));
    ASSERT_TRUE(std::filesystem::is_directory(db_path));

    bitcask::BitcaskHandle handle(db_path);
    EXPECT_EQ(handle.get_db_path(), db_path);
}


TEST(BitcaskHandleConstructor, NonExistentPath)
{
    EXPECT_THROW(bitcask::BitcaskHandle handle("/home/non/existent"), std::invalid_argument);
}

