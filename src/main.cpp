#include <iostream>
#include <string>

#include "log_parser.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: authwatch <logfile>\n";
        return 1;
    }

    const std::string path = argv[1];
    const int lines = count_lines(path);
    if (lines < 0) {
        std::cerr << "Error: cannot open " << path << "\n";
        return 1;
    }

    std::cout << path << ": " << lines << " lines\n";
    return 0;
}
