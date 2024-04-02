/**
 * @file bitcask_handle.cpp
 * @author Lucas
 * @brief
 * @version 0.1
 * @date 2024-03-28
 */

#include "bitcask_handle.hpp"

namespace bitcask
{

BitcaskHandle::BitcaskHandle(const std::string &db_path) : db_path_(db_path), store_(db_path)
{
    if (!std::filesystem::exists(db_path))
    {
        throw std::invalid_argument("Directory path does not exist");
    }
    if (!std::filesystem::is_empty(db_path))
    {
        // If the directory is not empty, load the keydir
        if (store_.load_keydir().ok())
        {
            // Successfully loaded keydir in green
            std::cout << "\033[32;1mSucessfully loaded keydir\033[0m" << std::endl;
        }
        else
        {
            throw std::runtime_error("Error loading keydir");
        }
    }
}

absl::Status BitcaskHandle::set(const std::string &key, const std::string &value)
{
    return store_.set(key, value);
}

absl::StatusOr<std::string> BitcaskHandle::get(const std::string &key)
{
    return store_.get(key);
}

absl::Status BitcaskHandle::del(const std::string &key)
{
    return store_.del(key);
}

absl::Status BitcaskHandle::list() const
{
    return store_.list();
}

void BitcaskHandle::help() const
{
    std::cout << "Available commands:" << '\n';
    std::cout << "  \033[1mget\033[0m <key>\t\tRetrieve the value for a key" << '\n';
    std::cout << "  \033[1mset\033[0m <key> <value>\tStore a key-value pair" << '\n';
    std::cout << "  \033[1mdel\033[0m <key>\t\tRemove a key-value pair" << '\n';
    std::cout << "  \033[1mlist\033[0m\tList all key-value pairs (or 'l' or 'keys' or 'k')" << '\n';
    std::cout << "  \033[1mhelp\033[0m\tDisplay this help message" << '\n';
    std::cout << "  \033[1mquit\033[0m\tExit the program" << '\n';
}

} // namespace bitcask