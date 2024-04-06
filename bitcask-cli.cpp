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

    // Print welcome message
    std::cout << "Welcome to \033[31;1mBitcask CLI\033[0m!" << '\n';
    std::cout << "Type 'help' for a list of commands." << '\n';
    std::cout << "Type 'quit' to exit." << '\n';

    // Create a BitcaskHandle object
    std::string db_path = argv[1];
    bitcask::BitcaskHandle handle = bitcask::BitcaskHandle(db_path);


    // Main loop
    while (true)
    {
        std::string input;
        std::cout << "\033[1m> \033[0m"; // Prompt
        std::getline(std::cin, input);

        if (input == "quit" || input == "q")
        {
            break;
        }
        else if (input == "help")
        {
            handle.help();
        }
        else
        {
            // Parse the input
            std::string command;
            std::string key;
            std::string value;

            std::istringstream iss(input);
            iss >> command;

            if (command == "get" || command == "g")
            {
                iss >> key;
                absl::StatusOr<std::string> result = handle.get(key);
                if (result.ok())
                {
                    std::cout << "Value: " << result.value() << '\n';
                }
                else
                {
                    std::cerr << "Error: " << result.status().message() << std::endl;
                }
            }
            else if (command == "set" || command == "s")
            {
                iss >> key;
                iss >> value;
                // std::cout << "key=" << key;
                // std::cout << ", value=" << value << '\n';
                absl::Status status = handle.set(key, value);
                if (!status.ok())
                {
                    std::cerr << "Error: " << status.message() << std::endl;
                }
            }
            else if (command == "del" || command == "d")
            {
                iss >> key;
                // std::cout << "key=" << key << '\n';
                absl::Status status = handle.del(key);
                if (!status.ok())
                {
                    std::cerr << "Error: " << status.message() << std::endl;
                }
            }
            else if (command == "list" || command == "l" || command == "keys" || command == "k")
            {
                absl::Status status = handle.list();
                if (!status.ok())
                {
                    std::cerr << "Error: " << status.message() << std::endl;
                }
            }
            else
            {
                std::cerr << "Unrecognized command: " << input << std::endl;
            }
        }
    }

    return EXIT_SUCCESS;
}
