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
 * @struct DatafileEntry
 * @brief Represents an entry in a datafile.
 */
struct DatafileEntry
{
    char CRC[4];                   /**< Cyclical Redundancy Check */
    std::uint16_t ksz;             /**< key size */
    std::uint16_t vsz;             /**< value size */
    std::unique_ptr<char[]> key;   /**< key */
    std::unique_ptr<char[]> value; /**< value */

    DatafileEntry()
    {
        key = nullptr;
        value = nullptr;
    }
    DatafileEntry(const uint16_t ksz, const uint16_t vsz) : ksz(ksz), vsz(vsz)
    {
        key = std::make_unique<char[]>(ksz);
        value = std::make_unique<char[]>(vsz);
    }
    ~DatafileEntry() = default;

    void set_ksz(const uint16_t ksz)
    {
        this->ksz = ksz;
        this->key = std::make_unique<char[]>(ksz);
    }
    void set_vsz(const uint16_t vsz)
    {
        this->vsz = vsz;
        this->value = std::make_unique<char[]>(vsz);
    }
};

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
    static const int CRC_SIZE;
    static const int KSZ_SIZE;
    static const int VSZ_SIZE;

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

    /**
     * @brief Read an entry from a datafile and return the value.
     *
     * @param file The file stream to read from.
     * @param vsz The size of the value.
     * @return absl::StatusOr<std::string> The value read from the file or an
     *        error if the value could not be read.
     */
    absl::StatusOr<std::string> read_value(std::ifstream &file, std::uint16_t vsz) const;

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