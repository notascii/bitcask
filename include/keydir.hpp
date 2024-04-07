/**
 * @file keydir.hpp
 * @author Lucas
 * @brief
 * @version 0.1
 * @date 2024-03-30
 */

#ifndef BITCASK_KEYDIR_HPP_
#define BITCASK_KEYDIR_HPP_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <cstdint>
#include <string>
#include <unordered_map>

using fileid_t = uint32_t; // -> TODO: bad practice ? fileid_t is also defined
                           // in store.hpp. I don't know how to use the one
                           // defined in store.hpp here without including
                           // store.hpp and creating a circular dependency.

namespace keydir
{

/**
 * @struct Entry
 * @brief Represents entry in the keydir. Contains file id, value size, and
 * value position. Also provides an equality operator to compare two entries.
 */
struct Entry
{
    fileid_t fileid;
    std::uint16_t vsz;
    std::uint16_t vpos;

    bool operator==(const Entry &other) const
    {
        return fileid == other.fileid && vsz == other.vsz && vpos == other.vpos;
    }
};

/**
 * @class KeyDir
 * @brief Represents the keydir. Contains an unordered map of key (std::string)
 * to entries (store::keydir::Entry) and provides methods to set, get, and
 * delete pairs of key and entry. Also provides methods to get the size of the
 * keydir and to iterate over the keydir.
 */
class KeyDir
{
  private:
    std::unordered_map<std::string, Entry> keydir_; // UnorderedMap[key: string] -> KeyDirEntry

  public:
    KeyDir();

    absl::Status set(const std::string &key, const Entry &entry);
    absl::StatusOr<Entry> get(const std::string &key) const;
    absl::Status del(const std::string &key);

    inline std::size_t size() const
    {
        return keydir_.size();
    }

    inline auto begin() const
    {
        return keydir_.begin();
    }

    inline auto end() const
    {
        return keydir_.end();
    }
    inline auto find(const std::string &key) const
    {
        return keydir_.find(key);
    }
};

} // namespace keydir

#endif // BITCASK_KEYDIR_HPP_