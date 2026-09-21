#include "detector.hpp"

BruteForceDetector::BruteForceDetector(int threshold, int window_seconds)
    : threshold_(threshold), window_seconds_(window_seconds) {}

std::optional<Alert> BruteForceDetector::record_failure(const std::string& ip,
                                                        std::time_t when) {
    auto& times = failures_[ip];
    times.push_back(when);

    // Drop failures that have fallen out of the window.
    while (!times.empty() && when - times.front() > window_seconds_) {
        times.pop_front();
    }

    if (static_cast<int>(times.size()) >= threshold_) {
        Alert alert{ip, static_cast<int>(times.size()), times.front(), times.back()};
        times.clear();
        return alert;
    }
    return std::nullopt;
}
