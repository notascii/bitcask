/**
 * @file keydir.cpp
 * @author Lucas
 * @brief
 * @version 0.1
 * @date 2024-04-01
 */

#include "keydir.hpp"

namespace keydir
{

KeyDir::KeyDir()
{
    std::unordered_map<std::string, Entry> keydir_;
}

absl::Status KeyDir::set(const std::string &key, const Entry &entry)
{
    keydir_[key] = entry;

    return absl::OkStatus();
}

absl::StatusOr<Entry> KeyDir::get(const std::string &key) const
{
    if (keydir_.find(key) == keydir_.end())
    {
        return absl::NotFoundError("Key: " + key + " not found");
    }
    return keydir_.at(key);
}

absl::Status KeyDir::del(const std::string &key)
{
    if (keydir_.find(key) == keydir_.end())
    {
        return absl::NotFoundError("Key: " + key + " not found");
    }
    keydir_.erase(key);

    return absl::OkStatus();
}

} // namespace keydir