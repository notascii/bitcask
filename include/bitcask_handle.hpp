/**
 * @file bitcask_handle.hpp
 * @author Lucas
 * @brief BitcaskHandle class declaration
 * @version 0.1
 * @date 2024-03-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef BITCASK_BITCASK_HANDLE_HPP_
#define BITCASK_BITCASK_HANDLE_HPP_

#include <iostream>
#include <filesystem>


/** \namespace bitcask */
namespace bitcask {

class BitcaskHandle {
private:
    std::string db_path_; /** Path to the database directory */
public:
    explicit BitcaskHandle(const std::string &db_path);

    void get(const std::string &key);
    void set(const std::string &key, const std::string &value);
    void del(const std::string &key);
    void list();
    void help();
    void quit();

    std::string get_db_path() const { return db_path_; }
};

} // namespace bitcask


#endif // BITCASK_BITCASK_HANDLE_HPP_