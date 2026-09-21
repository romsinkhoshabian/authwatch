#include <gtest/gtest.h>

#include "detector.hpp"

TEST(Detector, AlertsAtThreshold) {
    BruteForceDetector d(5, 60);
    const std::time_t t0 = 1000000;
    for (int i = 0; i < 4; ++i) {
        EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + i).has_value());
    }
    auto alert = d.record_failure("1.2.3.4", t0 + 4);
    ASSERT_TRUE(alert.has_value());
    EXPECT_EQ(alert->ip, "1.2.3.4");
    EXPECT_EQ(alert->failures, 5);
    EXPECT_EQ(alert->first_seen, t0);
    EXPECT_EQ(alert->last_seen, t0 + 4);
}

TEST(Detector, NoAlertWhenSpreadBeyondWindow) {
    BruteForceDetector d(5, 60);
    const std::time_t t0 = 1000000;
    for (int i = 0; i < 10; ++i) {
        // One failure every 30 seconds: never 5 inside any 60-second window.
        EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + i * 30).has_value());
    }
}

TEST(Detector, TracksIpsIndependently) {
    BruteForceDetector d(3, 60);
    const std::time_t t0 = 1000000;
    EXPECT_FALSE(d.record_failure("1.1.1.1", t0).has_value());
    EXPECT_FALSE(d.record_failure("2.2.2.2", t0 + 1).has_value());
    EXPECT_FALSE(d.record_failure("1.1.1.1", t0 + 2).has_value());
    EXPECT_FALSE(d.record_failure("2.2.2.2", t0 + 3).has_value());
    auto alert = d.record_failure("1.1.1.1", t0 + 4);
    ASSERT_TRUE(alert.has_value());
    EXPECT_EQ(alert->ip, "1.1.1.1");
}

TEST(Detector, OneBurstGivesOneAlert) {
    BruteForceDetector d(3, 60);
    const std::time_t t0 = 1000000;
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0).has_value());
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + 1).has_value());
    EXPECT_TRUE(d.record_failure("1.2.3.4", t0 + 2).has_value());
    // History was cleared, so the next failure starts a new count.
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + 3).has_value());
}

TEST(Detector, OldFailuresAgeOut) {
    BruteForceDetector d(3, 10);
    const std::time_t t0 = 1000000;
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0).has_value());
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + 1).has_value());
    // 20s later: the first two have aged out, so this is failure #1 again.
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + 21).has_value());
    EXPECT_FALSE(d.record_failure("1.2.3.4", t0 + 22).has_value());
    EXPECT_TRUE(d.record_failure("1.2.3.4", t0 + 23).has_value());
}
