/**
 * @file store.hpp
 * @author Lucas
 * @brief
 * @version 0.1
 * @date 2024-03-29
 */

#ifndef BITCASK_STORE_HPP_
#define BITCASK_STORE_HPP_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "keydir.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace store
{

using fileid_t = uint32_t;
namespace fs = std::filesystem;

/**
 * @class Store
 * @brief Represents the store. The store manages the keydir and the datafiles.
 *        It provides methods to set, get, and delete key-value pairs, as well
 *        as to list all key-value pairs.
 */
class Store
{
  private:
    std::string db_path_;
    fileid_t active_fileid_;
    uint16_t active_file_offset_;
    keydir::KeyDir keydir_;
    static const std::uint64_t TOMBSTONE;
    static const int TOMBSTONE_SIZE;

  public:
    Store(const std::string &db_path);

    absl::Status set(const std::string &key, const std::string &value);
    absl::StatusOr<std::string> get(const std::string &key) const;
    absl::Status del(const std::string &key);
    absl::Status list() const;

    /**
     * @brief Populate the keydir with the entries from the datafiles, if any
     *        exist.
     *
     * @return absl::Status Status::OK if the keydir was successfully loaded.
     *         Or absl::InternalError if there was an error opening a datafile.
     */
    absl::Status load_keydir();

    inline uint32_t active_fileid() const
    {
        return static_cast<uint32_t>(active_fileid_);
    }

    inline uint16_t active_file_offset() const
    {
        return active_file_offset_;
    }

    inline std::size_t kd_size() const
    {
        return keydir_.size();
    }

    inline absl::StatusOr<keydir::Entry> kd_get(const std::string &key) const
    {
        return keydir_.get(key);
    }

    inline fs::path active_datafile_path() const
    {
        return fs::path(db_path_) / fs::path("datafile" + std::to_string(active_fileid_));
    }
};

} // namespace store

#endif // BITCASK_STORE_HPP_