#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "detector.hpp"
#include "log_parser.hpp"

namespace {

int current_year() {
    const std::time_t now = std::time(nullptr);
    std::tm tm{};
    gmtime_r(&now, &tm);
    return tm.tm_year + 1900;
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: authwatch <logfile> [threshold=5] [window_seconds=60]\n";
        return 1;
    }

    const std::string path = argv[1];
    int threshold = 5;
    int window_seconds = 60;
    try {
        if (argc >= 3) threshold = std::stoi(argv[2]);
        if (argc >= 4) window_seconds = std::stoi(argv[3]);
    } catch (const std::exception&) {
        std::cerr << "Error: threshold and window_seconds must be integers\n";
        return 1;
    }
    if (threshold < 1 || window_seconds < 1) {
        std::cerr << "Error: threshold and window_seconds must be at least 1\n";
        return 1;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open " << path << "\n";
        return 1;
    }

    const int year = current_year();
    BruteForceDetector detector(threshold, window_seconds);

    int events = 0;
    int alerts = 0;
    std::string line;
    while (std::getline(file, line)) {
        const auto ev = parse_line(line);
        if (!ev) {
            continue;
        }
        ++events;

        if (ev->type == EventType::AcceptedPassword) {
            continue;
        }
        const auto when = parse_timestamp(ev->timestamp, year);
        if (!when) {
            continue;
        }

        const auto alert = detector.record_failure(ev->ip, *when);
        if (alert) {
            ++alerts;
            std::cout << "{\"alert\":\"brute_force\",\"ip\":\"" << alert->ip
                      << "\",\"failures\":" << alert->failures
                      << ",\"first_seen\":" << alert->first_seen
                      << ",\"last_seen\":" << alert->last_seen << "}\n";
        }
    }

    std::cerr << events << " events parsed, " << alerts << " alert(s)\n";
    return 0;
}
