# ---- Stage 1: build and test ----
FROM debian:trixie-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ cmake make ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt .
COPY src/ src/
COPY tests/ tests/

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build \
    && ctest --test-dir build --output-on-failure

# ---- Stage 2: small runtime image ----
FROM debian:trixie-slim

RUN useradd --system --no-create-home authwatch
COPY --from=build /src/build/authwatch /usr/local/bin/authwatch

USER authwatch
ENTRYPOINT ["authwatch"]
