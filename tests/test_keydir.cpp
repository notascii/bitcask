#include <filesystem>
#include <gtest/gtest.h>
#include "store.hpp"


/*
We want to ensure that the keydir is working as expected. We will use
class store::Store to interact with the keydir as it the class that creates
and manages the keydir (i.e. automatically create KeyDir entries using
store::Store::set and uses store::Store methods to interact with the keydir:
store::Store::kd_get, store::Store::kd_size, etc.)
*/


namespace fs = std::filesystem;

static const std::string base_path = "./tests/test_keydir_";
static const std::string paths[] = {
    "simple_entry",
    "set_different_value_size",
    "set_and_get_multiple",
    "update",
    "delete",
    "delete_unexisting",
};

class KeyDir : public ::testing::Test {
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

TEST_F(KeyDir, SimpleEntry)
{
    store::Store store(base_path + paths[0]);
    keydir::Entry expected_entry = {.fileid = 1, .vsz = 1, .vpos = 0};

    absl::Status status = store.set("a", "1");
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(store.kd_size(), 1);

    keydir::Entry actual_entry = store.kd_get("a").value();
    EXPECT_EQ(typeid(actual_entry), typeid(keydir::Entry));

    EXPECT_EQ(actual_entry.fileid, expected_entry.fileid);
    EXPECT_EQ(actual_entry.vsz, expected_entry.vsz);
    EXPECT_EQ(actual_entry.vpos, expected_entry.vpos);
}


TEST_F(KeyDir, GetNonExistentKey)
{
    keydir::KeyDir kd;

    absl::StatusOr<keydir::Entry> entry = kd.get("a");
    EXPECT_EQ(entry.status().code(), absl::StatusCode::kNotFound);
}


TEST_F(KeyDir, SetDifferentValueSize)
{
    store::Store store(base_path + paths[1]);
    keydir::Entry actual_entry, expected_entry;
    absl::Status status;

    // Set a key with a size of 2
    expected_entry = {.fileid = 1, .vsz = 2, .vpos = 0};
    status = store.set("a", "11");
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.kd_size(), 1);

    actual_entry = store.kd_get("a").value();
    EXPECT_EQ(actual_entry.fileid, expected_entry.fileid);
    EXPECT_EQ(actual_entry.vsz, expected_entry.vsz);
    EXPECT_EQ(actual_entry.vpos, expected_entry.vpos);
}


TEST_F(KeyDir, SetAndGetMultiple)
{
    store::Store store(base_path + paths[2]);
    keydir::Entry expected_entry;
    absl::Status status;

    EXPECT_EQ(store.kd_size(), 0);

    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.kd_size(), 1);

    status = store.set("b", "2");
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.kd_size(), 2);

    expected_entry = {.fileid = 1, .vsz = 1, .vpos = 0};
    keydir::Entry actual_entry = store.kd_get("a").value();
    EXPECT_EQ(typeid(actual_entry), typeid(keydir::Entry));

    EXPECT_EQ(actual_entry.fileid, expected_entry.fileid);
    EXPECT_EQ(actual_entry.vsz, expected_entry.vsz);
    EXPECT_EQ(actual_entry.vpos, expected_entry.vpos);

    expected_entry = {.fileid = 1, .vsz = 1, .vpos = 12};
    actual_entry = store.kd_get("b").value();
    EXPECT_EQ(actual_entry.fileid, expected_entry.fileid);
    EXPECT_EQ(actual_entry.vsz, expected_entry.vsz);
    EXPECT_EQ(actual_entry.vpos, expected_entry.vpos);
}


TEST_F(KeyDir, Update)
{
    store::Store store(base_path + paths[3]);
    absl::Status status;
    keydir::Entry actual_entry, expected_entry;

    // Set "a": "1"
    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.kd_size(), 1);

    expected_entry = {.fileid = 1, .vsz = 1, .vpos = 0};
    actual_entry = store.kd_get("a").value();
    EXPECT_EQ(typeid(actual_entry), typeid(keydir::Entry));
    EXPECT_EQ(actual_entry, expected_entry);

    // Update "a": "foo"
    status = store.set("a", "foo");
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(store.kd_size(), 1);

    expected_entry = {.fileid = 1, .vsz = 3, .vpos = 12};
    actual_entry = store.kd_get("a").value();
    EXPECT_EQ(typeid(actual_entry), typeid(keydir::Entry));
    EXPECT_EQ(actual_entry, expected_entry);
}


TEST_F(KeyDir, Delete)
{
    store::Store store(base_path + paths[4]);
    absl::Status status;
    keydir::Entry actual_entry, expected_entry;

    status = store.set("a", "1");
    ASSERT_TRUE(status.ok());
    expected_entry = {.fileid = 1, .vsz = 1, .vpos = 0};
    actual_entry = store.kd_get("a").value();
    EXPECT_EQ(actual_entry, expected_entry);

    status = store.set("b", "2");
    ASSERT_TRUE(status.ok());
    expected_entry = {.fileid = 1, .vsz = 1, .vpos = 12};
    actual_entry = store.kd_get("b").value();
    EXPECT_EQ(actual_entry, expected_entry);

    EXPECT_EQ(store.kd_size(), 2);

    status = store.del("a");
    ASSERT_TRUE(status.ok());

    EXPECT_EQ(store.kd_size(), 1);

    absl::StatusOr<keydir::Entry> entry = store.kd_get("a");
    EXPECT_EQ(entry.status().code(), absl::StatusCode::kNotFound);
}


TEST_F(KeyDir, DeleteUnexisting)
{
    store::Store store(base_path + paths[5]);
    absl::Status status;

    status = store.del("b");
    EXPECT_EQ(status.code(), absl::StatusCode::kNotFound);
}
