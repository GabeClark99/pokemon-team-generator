FROM gcc:14 AS dependencies

RUN apt-get update && apt-get install -y cmake git gdb valgrind pkg-config && \
    rm -rf /var/lib/apt/lists/*

# manually install Catch2
WORKDIR /tmp
RUN git clone --branch v3.8.1 https://github.com/catchorg/Catch2.git
WORKDIR /tmp/Catch2
RUN cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_TESTING=OFF
RUN cmake --build build --target install -- -j$(nproc)

# Manually install nlohmann/json
WORKDIR /tmp
RUN git clone --branch v3.12.0 https://github.com/nlohmann/json.git
WORKDIR /tmp/json
RUN cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build --target install -- -j$(nproc)

FROM dependencies AS build

WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -- -j$(nproc)

FROM build AS run

WORKDIR /app/build
# CMD ["ctest", "--output-on-failure"]
CMD ctest --output-on-failure && ./team_builder
