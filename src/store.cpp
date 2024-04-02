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

Store::Store(const std::string &db_path) : db_path_(db_path), active_fileid_(1), active_file_offset_(0), keydir_()
{
}

absl::Status Store::set(const std::string &key, const std::string &value)
{
    // open the active file for writing in append mode
    std::ofstream file;
    file.open(active_datafile_path(), std::ios::app);
    // std::cout << "active_datafile_path: " << active_datafile_path() << std::endl;

    if (!file.is_open())
    {
        return absl::InternalError("Error opening file");
    }

    // write the key and value to the datafile
    constexpr absl::string_view entry = "CRC,%d,%d,%s,%s\n";
    std::string entry_str = absl::StrFormat(entry, key.size(), value.size(), key, value);

    file.write(entry_str.c_str(), entry_str.size());
    // cast the key to a char array and write it to the file
    // std::vector<std::uint8_t> key_bytes(key.begin(), key.end());
    // file.write(reinterpret_cast<const char*>(key_bytes.data()), key_bytes.size());
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
    keydir::Entry entry_info = {active_fileid_, static_cast<std::uint16_t>(value.size()), active_file_offset_};
    absl::Status status = keydir_.set(key, entry_info);

    // increment the offset as the write and keydir update were successful
    active_file_offset_ += entry_str.size();

    return status;
}

absl::StatusOr<std::string> Store::get(const std::string &key) const
{
    // get the entry from the keydir
    absl::StatusOr<keydir::Entry> entry = keydir_.get(key);
    if (!entry.ok())
    {
        return entry.status();
    }

    // std::cout << "entry: " << entry->fileid << " " << entry->vsz << " " << entry->vpos << std::endl;

    // open the file for reading
    std::ifstream file;
    file.open(active_datafile_path(), std::ios::in);

    if (!file.is_open())
    {
        return absl::InternalError("Error opening file");
    }

    // seek to the position of the value in the file
    file.seekg(entry->vpos); // TODO: doesnt work, it jumps "vpos" bytes from the beginning of the file
                             // but was intended to jump "vpos" lines from the beginning of the file.

    // read the current line of the file
    std::string line = "";
    std::getline(file, line);

    std::size_t pos = line.find_last_of(',');
    if (pos == std::string::npos)
    {
        return absl::InternalError("Error reading from file");
    }

    std::string value = line.substr(pos + 1);
    if (value.size() != entry->vsz)
    {
        return absl::InternalError("Error reading from file");
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
    // open the file for writing in append mode
    std::ofstream file;
    file.open(active_datafile_path(), std::ios::app);

    if (!file.is_open())
    {
        return absl::InternalError("Error opening file");
    }

    // write the key and TOMSTONE to the datafile
    constexpr absl::string_view entry = "CRC,%d,%d,%s,%lu\n";
    std::string entry_str = absl::StrFormat(entry, key.size(), TOMBSTONE_SIZE, key, TOMBSTONE);

    file.write(entry_str.c_str(), entry_str.size());
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

    // remove the key from the keydir
    absl::Status status = keydir_.del(key);

    // increment the offset as the write and keydir update were successful
    active_file_offset_ += entry_str.size();

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

    // read the file line by line
    std::string line;
    while (std::getline(file, line))
    {
        // split the comma separated line
        std::vector<std::string> parts = absl::StrSplit(line, ',');
        std::string key = parts[3], value = parts[4];

        // don't add keys with tombstone values to the keydir
        if (value == std::to_string(TOMBSTONE))
        {
            // remove the key from the keydir in case it was added before
            absl::Status status = keydir_.del(key);
            // increment the offset
            active_file_offset_ += line.size() + 1; // +1 for the newline character
            continue;
        }

        // update the keydir
        keydir::Entry entry_info = {active_fileid_, static_cast<std::uint16_t>(value.size()), active_file_offset_};
        absl::Status status = keydir_.set(key, entry_info);

        // increment the offset
        active_file_offset_ += line.size() + 1; // +1 for the newline character
    }

    // close the file
    file.close();

    return absl::OkStatus();
}

// absl::StatusOr<std::ofstream> Store::create_writer(fileid_t fileid)
// {
//     // TODO: implement this function
//     return absl::Status(absl::StatusCode::kUnimplemented, "Not implemented");
// }

} // namespace store
