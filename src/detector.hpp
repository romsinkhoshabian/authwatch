#pragma once

#include <ctime>
#include <deque>
#include <optional>
#include <string>
#include <unordered_map>

struct Alert {
    std::string ip;
    int         failures;
    std::time_t first_seen;
    std::time_t last_seen;
};

// Flags source IPs that fail too many logins within a sliding time window.
// Events must be fed in chronological order.
class BruteForceDetector {
public:
    BruteForceDetector(int threshold, int window_seconds);

    // Records one failed login from `ip` at time `when`. Returns an Alert when
    // this failure brings the count inside the window up to the threshold.
    // After an alert, that IP's history is cleared, so one burst gives one alert.
    std::optional<Alert> record_failure(const std::string& ip, std::time_t when);

private:
    int threshold_;
    int window_seconds_;
    std::unordered_map<std::string, std::deque<std::time_t>> failures_;
};
