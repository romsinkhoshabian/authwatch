#include <fstream>
#include <iostream>
#include <string>

#include "log_parser.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: authwatch <logfile>\n";
        return 1;
    }

    const std::string path = argv[1];
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open " << path << "\n";
        return 1;
    }

    int parsed = 0;
    std::string line;
    while (std::getline(file, line)) {
        const auto ev = parse_line(line);
        if (!ev) {
            continue;
        }
        ++parsed;
        std::cout << ev->timestamp << " | " << ev->user << " | "
                  << ev->ip << " | " << ev->port << "\n";
    }

    std::cout << parsed << " events parsed\n";
    return 0;
}
