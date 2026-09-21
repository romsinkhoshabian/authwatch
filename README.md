# authwatch

A small C++17 command-line tool that reads an SSH authentication log and flags
brute-force activity: too many failed logins from one source IP within a
sliding time window. Written from scratch with CMake and GoogleTest, and
packaged as a multi-stage Docker image.

## What it does

- Parses sshd lines from an `auth.log`-style file (failed password, failed
  password for an invalid user, invalid user, accepted password) and ignores
  everything else.
- Converts syslog timestamps (which carry no year) into UTC epoch seconds.
- Tracks failed attempts per source IP in a sliding window
  (`unordered_map` of IP to `deque` of timestamps). When an IP reaches the
  threshold inside the window, it emits one JSON alert and resets that IP.
- Alerts go to stdout as JSON lines; the summary goes to stderr, so output can
  be piped into other tools.

Example:

    $ authwatch data/sample_auth.log
    {"alert":"brute_force","ip":"198.51.100.23","failures":5,"first_seen":1789813201,"last_seen":1789813210}
    15 events parsed, 1 alert(s)

## Usage

    authwatch <logfile> [threshold=5] [window_seconds=60]

## Build and test (local)

    cmake -S . -B build
    cmake --build build
    ./build/authwatch_tests
    ./build/authwatch data/sample_auth.log

Requires a C++17 compiler and CMake 3.16+. GoogleTest is fetched automatically.

## Docker

The Dockerfile is multi-stage. The first stage compiles the code and runs the
unit tests, so an image only builds if the tests pass. The second stage is a
slim Debian runtime image containing just the binary, running as a non-root
user.

    docker build -t authwatch:0.2 .
    docker run --rm -v "$PWD/data:/data:ro" authwatch:0.2 /data/sample_auth.log

(On SELinux systems such as Fedora, use `:ro,Z` on the volume mount.)

## Project layout

    src/log_parser.*   log line parsing and timestamp conversion
    src/detector.*     sliding-window brute-force detector
    src/main.cpp       CLI
    tests/             GoogleTest unit tests (parser, timestamps, detector)
    data/              sample log with a brute-force burst and edge cases
    Dockerfile         multi-stage build

## Known limitations

- Syslog timestamps have no year, so the current year is assumed; a log that
  spans a year boundary will mis-order events.
- Events must be in chronological order.
- Parsing uses `std::regex`, which is simple but not fast.

## Planned

- Follow mode (tail a live log)
- Sanitizer runs (ASan/UBSan/TSan) and a regex vs hand-written parser benchmark
- Docker Compose lab with an sshd target and an attacker container
- CI with GitHub Actions
