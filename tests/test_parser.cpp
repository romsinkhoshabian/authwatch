#include <gtest/gtest.h>

#include "log_parser.hpp"

TEST(ParseLine, FailedPasswordValidUser) {
    auto ev = parse_line(
        "Sep 19 10:15:35 host sshd[1234]: Failed password for root "
        "from 203.0.113.5 port 52146 ssh2");
    ASSERT_TRUE(ev.has_value());
    EXPECT_EQ(ev->type, EventType::FailedPassword);
    EXPECT_EQ(ev->timestamp, "Sep 19 10:15:35");
    EXPECT_EQ(ev->user, "root");
    EXPECT_EQ(ev->ip, "203.0.113.5");
    EXPECT_EQ(ev->port, 52146);
}

TEST(ParseLine, FailedPasswordInvalidUser) {
    auto ev = parse_line(
        "Sep 19 10:15:32 host sshd[1234]: Failed password for invalid user "
        "admin from 203.0.113.5 port 52144 ssh2");
    ASSERT_TRUE(ev.has_value());
    EXPECT_EQ(ev->type, EventType::FailedPassword);
    EXPECT_EQ(ev->user, "admin");
    EXPECT_EQ(ev->port, 52144);
}

TEST(ParseLine, SpacePaddedDay) {
    auto ev = parse_line(
        "Sep  3 09:01:02 host sshd[900]: Failed password for root "
        "from 203.0.113.77 port 33333 ssh2");
    ASSERT_TRUE(ev.has_value());
    EXPECT_EQ(ev->timestamp, "Sep  3 09:01:02");
    EXPECT_EQ(ev->ip, "203.0.113.77");
}

TEST(ParseLine, IgnoresNonSshdLine) {
    auto ev = parse_line(
        "Sep 19 10:24:01 host CRON[2201]: pam_unix(cron:session): "
        "session opened for user root(uid=0) by (uid=0)");
    EXPECT_FALSE(ev.has_value());
}
