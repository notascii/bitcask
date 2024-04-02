#include <filesystem>
#include <gtest/gtest.h>
#include "store.hpp"


namespace fs = std::filesystem;

static const std::string base_path = "./tests/test_store_";
static const std::string paths[] = {
    "set",
    "set_multiple",
    "get",
    "load_keydir",
    "del",
    "load_keydir_with_tombstone",
};

class Store : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        // Create directories
        for (const std::string &path : paths) {
            fs::create_directories(base_path + path);
        }
    }

    static void TearDownTestCase() {
        // Remove directories
        for (const std::string &path : paths) {
            fs::remove_all(base_path + path);
        }
    }
};


TEST(StoreConstructor, Default)
{
    store::Store store("/we/dont/need/a/path/for/this/test");

    EXPECT_EQ(store.kd_size(), 0);
    EXPECT_EQ(store.active_fileid(), 1);
    EXPECT_EQ(store.active_file_offset(), 0);
}


TEST_F(Store, Set)
{
    store::Store store(base_path + paths[0]);
    absl::Status status;

    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(store.kd_size(), 1);
    EXPECT_EQ(store.active_fileid(), 1);
    EXPECT_EQ(store.active_file_offset(), 12); // 12 = size of "CRC,1,1,a,1\n"
}


TEST_F(Store, SetMultiple)
{
    store::Store store(base_path + paths[1]);
    absl::Status status;

    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());
    status = store.set("b", "2");
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(store.kd_size(), 2);
    EXPECT_EQ(store.active_fileid(), 1);
    EXPECT_EQ(store.active_file_offset(), 24); // 24 = size of "CRC,1,1,a,1\nCRC,1,1,b,2\n"
}


TEST_F(Store, Get)
{
    store::Store store(base_path + paths[2]);
    absl::Status status;

    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());

    absl::StatusOr<std::string> value = store.get("a");
    ASSERT_EQ(value.status(), absl::OkStatus());
    EXPECT_EQ(value.value(), "1");
}


TEST_F(Store, LoadKeydir)
{
    store::Store store(base_path + paths[3]);
    absl::Status status;

    // (1) Populate the keydir and datafile
    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());
    status = store.set("b", "test");
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.active_file_offset(), 27); // 27 = size of "CRC,1,1,a,1\nCRC,4,4,b,test\n"

    // (2) Creating a new store and load the keydir as an existing datafile
    //     is present in the directory
    store::Store store2(base_path + "load_keydir");
    status = store2.load_keydir();
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store2.kd_size(), 2);
    EXPECT_EQ(store2.active_fileid(), 1);
    EXPECT_EQ(store2.active_file_offset(), 27); // 27 = size of "CRC,1,1,a,1\nCRC,4,4,b,test\n"
}


TEST_F(Store, Del)
{
    store::Store store(base_path + paths[4]);
    absl::Status status;

    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());
    status = store.set("b", "2");
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(store.kd_size(), 2);

    status = store.del("a");
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(store.kd_size(), 1);

    // Check the datafile content
    std::ifstream file;
    file.open(store.active_datafile_path(), std::ios::in);

    auto TOMBSTONE = 0xDEADDEADDEADDEAD;

    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "CRC,1,1,a,1");
    std::getline(file, line);
    EXPECT_EQ(line, "CRC,1,1,b,2");
    std::getline(file, line);
    EXPECT_EQ(line, "CRC,1,20,a," + std::to_string(TOMBSTONE));
}


TEST_F(Store, LoadKeydirWithTombstone)
{
    store::Store store(base_path + "load_keydir_with_tombstone");
    absl::Status status;

    // (1) Populate the keydir and datafile
    status = store.set("a", "1"); // size = 12
    ASSERT_TRUE(status.ok());
    status = store.set("b", "2"); // size = 12
    ASSERT_TRUE(status.ok());
    status = store.del("a"); // size = 32 = size of "CRC,1,20,a,16045725885737590445\n"
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.active_file_offset(), 56); // 56 = 12 + 12 + 32

    // (2) Creating a new store and load the keydir as an existing datafile
    //     is present in the directory
    store::Store store2(base_path + "load_keydir_with_tombstone");
    status = store2.load_keydir();
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store2.kd_size(), 1);
    EXPECT_EQ(store2.active_fileid(), 1);
    EXPECT_EQ(store.active_file_offset(), 56); // 56 = 12 + 12 + 32
}
