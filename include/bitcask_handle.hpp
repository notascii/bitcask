/**
 * @file bitcask_handle.hpp
 * @author Lucas
 * @brief BitcaskHandle class declaration
 * @version 0.1
 * @date 2024-03-28
 */

#ifndef BITCASK_BITCASK_HANDLE_HPP_
#define BITCASK_BITCASK_HANDLE_HPP_

#include "absl/status/status.h"
#include "store.hpp"
#include <filesystem>
#include <iostream>

namespace bitcask
{

/**
 * @class BitcaskHandle
 * @brief Represents the Bitcask handle. The handle provides methods to set, get,
 *        and delete key-value pairs, as well as to list all key-value pairs.
 */
class BitcaskHandle
{
  private:
    std::string db_path_; /** Path to the database directory */
    store::Store store_;  /** Store object to interact with the database */
  public:
    explicit BitcaskHandle(const std::string &db_path);

    /**
     * @brief Set a key-value pair in the database.
     *
     * @param key
     * @param value
     * @return absl::Status
     */
    absl::Status set(const std::string &key, const std::string &value);

    /**
     * @brief Get the value of a key from the database.
     *
     * @param key
     */
    absl::StatusOr<std::string> get(const std::string &key);

    /**
     * @brief Delete a key from the database.
     *
     * @param key
     */

    absl::Status del(const std::string &key);

    /**
     * @brief List all the keys in the database.
     *
     */
    absl::Status list() const;

    /**
     * @brief Display help message.
     *
     */
    void help() const;

    /**
     * @brief Quit the application.
     *
     */
    void quit();

    std::string get_db_path() const
    {
        return db_path_;
    }
};

} // namespace bitcask

#endif // BITCASK_BITCASK_HANDLE_HPP_