#include <iostream>

#include "parser.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./elfpeek <path_to_binary>\n";
        return 1;
    }

    try {
        Parser parser(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    
    return 0;
}
