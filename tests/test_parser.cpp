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

TEST(ParseLine, AcceptedPassword) {
    auto ev = parse_line(
        "Sep 19 10:22:00 host sshd[2020]: Accepted password for alice "
        "from 192.168.1.20 port 51234 ssh2");
    ASSERT_TRUE(ev.has_value());
    EXPECT_EQ(ev->type, EventType::AcceptedPassword);
    EXPECT_EQ(ev->timestamp, "Sep 19 10:22:00");
    EXPECT_EQ(ev->user, "alice");
    EXPECT_EQ(ev->ip, "192.168.1.20");
    EXPECT_EQ(ev->port, 51234);
}

TEST(ParseLine, InvalidUser) {
    auto ev = parse_line(
        "Sep 19 10:23:10 host sshd[2040]: Invalid user postgres from 198.51.100.23 port 40010");
    ASSERT_TRUE(ev.has_value());
    EXPECT_EQ(ev->type, EventType::InvalidUser);
    EXPECT_EQ(ev->timestamp, "Sep 19 10:23:10");
    EXPECT_EQ(ev->user, "postgres");
    EXPECT_EQ(ev->ip, "198.51.100.23");
    EXPECT_EQ(ev->port, 40010);
}

TEST(ParseLine, IgnoresEmptyLine) {
    auto ev = parse_line("");
    EXPECT_FALSE(ev.has_value());
}

TEST(ParseLine, IgnoresRandomText) {
    auto ev = parse_line("hello world");
    EXPECT_FALSE(ev.has_value());
}

TEST(ParseLine, RejectsFailedPasswordMissingPort) {
    auto ev = parse_line(
        "Sep 19 10:15:35 host sshd[1234]: Failed password for root "
        "from 203.0.113.5");
    EXPECT_FALSE(ev.has_value());
}

TEST(ParseTimestamp, ValidTimestamp) {
    auto t = parse_timestamp("Sep 19 10:20:01", 2026);
    ASSERT_TRUE(t.has_value());
    EXPECT_EQ(*t, 1789813201);
}

TEST(ParseTimestamp, SpacePaddedDay) {
    auto t = parse_timestamp("Sep  3 09:01:02", 2026);
    ASSERT_TRUE(t.has_value());
    EXPECT_EQ(*t, 1788426062);
}

TEST(ParseTimestamp, RejectsGarbage) {
    EXPECT_FALSE(parse_timestamp("hello world", 2026).has_value());
    EXPECT_FALSE(parse_timestamp("", 2026).has_value());
    EXPECT_FALSE(parse_timestamp("Sep 19 25:00:00", 2026).has_value());
}
