/**
 * @file bitcask_handle.cpp
 * @author Lucas
 * @brief 
 * @version 0.1
 * @date 2024-03-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "bitcask_handle.hpp"

namespace bitcask {

BitcaskHandle::BitcaskHandle(const std::string &db_path)
{
    if (!std::filesystem::exists(db_path))
    {
        throw std::invalid_argument("Directory path does not exist");
    }

    // if the path is a valid directory but not empty, we should throw an exception
    if (!std::filesystem::is_empty(db_path))
    {
        throw std::invalid_argument("Given directory for database path is not empty");
    }

    db_path_ = db_path;
}


} // namespace bitcask