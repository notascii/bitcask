/**
 * @file store.cpp
 * @author Lucas
 * @brief
 * @version 0.1
 * @date 2024-03-29
 */

#include "store.hpp"

namespace store
{

const std::uint64_t Store::TOMBSTONE = 0xDEADDEADDEADDEAD;
const int Store::TOMBSTONE_SIZE = 20;
const int Store::CRC_SIZE = 4;
const int Store::KSZ_SIZE = 2;
const int Store::VSZ_SIZE = 2;

Store::Store(const std::string &db_path) : db_path_(db_path), active_fileid_(1), active_file_offset_(0), keydir_()
{
}

absl::Status Store::set(const std::string &key, const std::string &value)
{
    // open the active file for writing in binary append mode
    std::ofstream file(active_datafile_path(), std::ios::binary | std::ios::app);
    if (!file.is_open())
    {
        return absl::InternalError("Failed to open file for writing");
    }

    // check the key and value sizes
    if (key.size() > std::numeric_limits<std::uint16_t>::max())
    {
        return absl::InvalidArgumentError("Key size is too large");
    }
    std::uint16_t ksz = static_cast<std::uint16_t>(key.size());
    if (value.size() > std::numeric_limits<std::uint16_t>::max())
    {
        return absl::InvalidArgumentError("Value size is too large");
    }
    std::uint16_t vsz = static_cast<std::uint16_t>(value.size());

    // create the datafile entry
    store::DatafileEntry df_entry(ksz, vsz);
    std::memcpy(df_entry.CRC, "0000", 4);

    // copy the key and value to the df_entry
    // /!\ valgrind writing 1 byte after ?
    std::memcpy(df_entry.key.get(), key.c_str(), ksz);
    std::memcpy(df_entry.value.get(), value.c_str(), vsz);

    // write the df_entry to the file
    file.write(reinterpret_cast<const char*>(&df_entry), sizeof(df_entry) - 2 * sizeof(std::unique_ptr<char[]>));
    file.write(reinterpret_cast<const char*>(df_entry.key.get()), ksz);
    file.write(reinterpret_cast<const char*>(df_entry.value.get()), vsz);
    if (file.fail())
    {
        return absl::InternalError("Writing to file failed");
    }

    // close the file
    file.close();
    if (!file.good())
    {
        return absl::InternalError("Error closing file");
    }

    // update the keydir
    keydir::Entry kd_entry = { .fileid = active_fileid_, .vsz = vsz, .vpos = active_file_offset_ };
    absl::Status status = keydir_.set(key, kd_entry);

    // increment the offset as the write and keydir update were successful
    active_file_offset_ += sizeof(df_entry) - 2 * sizeof(std::unique_ptr<char[]>) + ksz + vsz;

    return status;
}

absl::StatusOr<std::string> Store::get(const std::string &key) const
{
    // get the entry from the keydir
    absl::StatusOr<keydir::Entry> kd_entry = keydir_.get(key);
    if (!kd_entry.ok())
    {
        return kd_entry.status();
    }

    // open the file for reading
    std::ifstream file;
    file.open(active_datafile_path(), std::ios::in);

    if (!file.is_open())
    {
        return absl::InternalError("Error opening file");
    }

    // seek to the position of the entry in the datafile
    file.seekg(kd_entry->vpos);

    absl::StatusOr<std::string> value = read_value(file, kd_entry->vsz);
    if (!value.ok())
    {
        return value.status();
    }

    if (file.fail())
    {
        return absl::InternalError("Error reading from file");
    }

    // close the file
    file.close();
    if (!file.good())
    {
        return absl::InternalError("Error closing file");
    }

    return value;
}

absl::Status Store::del(const std::string &key)
{
    // open the file for writing in binary append mode
    std::ofstream file;
    file.open(active_datafile_path(), std::ios::binary | std::ios::app);

    if (!file.is_open())
    {
        return absl::InternalError("Error opening file");
    }

    // write the entry to the file
    absl::Status status = set(key, std::to_string(Store::TOMBSTONE));
    if (!status.ok())
    {
        return status;
    }

    // close the file
    file.close();
    if (!file.good())
    {
        return absl::InternalError("Error closing file");
    }

    // remove the key from the keydir
    status = keydir_.del(key);
    if (!status.ok())
    {
        return status;
    }

    // increment the offset as the write and keydir update were successful
    active_file_offset_ += Store::CRC_SIZE + Store::KSZ_SIZE + Store::VSZ_SIZE + key.size() + Store::TOMBSTONE_SIZE;

    return status;
}

absl::Status Store::list() const
{
    std::vector<std::string> keys;
    for (auto [key, entry] : keydir_)
    {
        keys.push_back(key);
    }
    if (keys.empty())
    {
        std::cout << "Empty" << std::endl;
        return absl::OkStatus();
    }
    std::string joined_keys = absl::StrJoin(keys, ", ");
    std::cout << absl::StrCat("Keys[", keys.size(), "]: ", joined_keys) << std::endl;
    return absl::OkStatus();
}

absl::Status Store::load_keydir()
{
    // open the active file for reading
    std::ifstream file;
    file.open(active_datafile_path(), std::ios::in);

    if (!file.is_open())
    {
        return absl::InternalError("Error opening file");
    }

    // get the file size
    file.seekg(0, std::ios::end);
    std::size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the entries from the file
    DatafileEntry cur_df_entry;
    while (active_file_offset_ < file_size)
    {
        // read the entry from the file
        file.read(reinterpret_cast<char*>(&cur_df_entry), sizeof(cur_df_entry) - 2 * sizeof(std::unique_ptr<char[]>));
        cur_df_entry.key = std::make_unique<char[]>(cur_df_entry.ksz);
        cur_df_entry.value = std::make_unique<char[]>(cur_df_entry.vsz);
        file.read(reinterpret_cast<char*>(cur_df_entry.key.get()), cur_df_entry.ksz);
        file.read(reinterpret_cast<char*>(cur_df_entry.value.get()), cur_df_entry.vsz);
        if (file.fail())
        {
            return absl::InternalError("Error reading from file");
        }

        std::string key_str = std::string(cur_df_entry.key.get(), cur_df_entry.ksz);
        std::string value_str = std::string(cur_df_entry.value.get(), cur_df_entry.vsz);

        // don't add keys with tombstone values to the keydir
        if (value_str == std::to_string(TOMBSTONE))
        {
            // remove the key from the keydir in case it was added before
            absl::Status status = keydir_.del(key_str);
            // increment the offset
            active_file_offset_ += Store::CRC_SIZE + Store::KSZ_SIZE + Store::VSZ_SIZE + cur_df_entry.ksz + cur_df_entry.vsz;
            continue;
        }

        // update the keydir
        keydir::Entry entry_info = {active_fileid_, static_cast<std::uint16_t>(cur_df_entry.vsz), active_file_offset_};
        absl::Status status = keydir_.set(key_str, entry_info);

        // increment the offset
        active_file_offset_ += Store::CRC_SIZE + Store::KSZ_SIZE + Store::VSZ_SIZE + cur_df_entry.ksz + cur_df_entry.vsz;
    }

    // close the file
    file.close();

    return absl::OkStatus();
}

absl::StatusOr<std::string> Store::read_value(std::ifstream &file, std::uint16_t vsz) const
{
    // seek to the position of the key size in the file
    file.seekg(Store::CRC_SIZE, std::ios::cur);
    if (file.fail())
    {
        return absl::InternalError("Failed to seek to value position");
    }

    // read the key size to get the position of the value
    std::uint16_t ksz;
    file.read(reinterpret_cast<char*>(&ksz), sizeof(ksz));

    // seek to the position of the value in the file
    file.seekg(Store::VSZ_SIZE + ksz, std::ios::cur);

    // read the value
    std::string value_str;
    value_str.resize(vsz);
    file.read(reinterpret_cast<char*>(&value_str[0]), vsz);
    if (file.fail())
    {
        return absl::InternalError("Failed to read value from file");
    }

    return value_str;
}

// absl::StatusOr<std::ofstream> Store::create_writer(fileid_t fileid)
// {
//     // TODO: implement this function
//     return absl::Status(absl::StatusCode::kUnimplemented, "Not implemented");
// }

} // namespace store
