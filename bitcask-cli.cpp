#include <iostream>
#include <sstream>

#include "include/bitcask_handle.hpp"

int main(int argc, const char *argv[])
{
    // Handle command line arguments
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_dir> [args]" << std::endl;
        return EXIT_FAILURE;
    }

    // Create a BitcaskHandle object
    // bitcask::BitcaskHandle handle;

    // Print welcome message
    std::cout << "Welcome to \033[31;1mBitcask CLI\033[0m!" << '\n';
    std::cout << "Type 'help' for a list of commands." << '\n';
    std::cout << "Type 'quit' to exit." << '\n';

    // Main loop
    while (true)
    {
        std::string input;
        std::cout << "\033[1m> \033[0m"; // Prompt
        std::getline(std::cin, input);

        if (input == "quit")
        {
            break;
        }
        else if (input == "help")
        {
            std::cout << "Available commands:" << '\n';
            std::cout << "  get <key> - Retrieve the value for a key" << '\n';
            std::cout << "  set <key> <value> - Store a key-value pair" << '\n';
            std::cout << "  del <key> - Remove a key-value pair" << '\n';
            std::cout << "  list - List all key-value pairs" << '\n';
            std::cout << "  help - Display this help message" << '\n';
            std::cout << "  quit - Exit the program" << '\n';
        }
        else
        {
            // Parse the input
            std::string command;
            std::string key;
            std::string value;

            std::istringstream iss(input);
            iss >> command;

            if (command == "get")
            {
                iss >> key;
                std::cout << "key=" << key << '\n';
            }
            else if (command == "set")
            {
                iss >> key;
                iss >> value;
                std::cout << "key=" << key;
                std::cout << ", value=" << value << '\n';
            }
            else if (command == "del")
            {
                iss >> key;
                std::cout << "key=" << key << '\n';
            }
            else if (command == "list")
            {
                // List all key-value pairs
            }
            else
            {
                std::cerr << "Unrecognized command: " << input << std::endl;
            }
        }
    }

    return EXIT_SUCCESS;
}
