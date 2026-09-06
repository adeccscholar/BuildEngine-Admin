cmake_minimum_required(VERSION 3.21)
project(harfbuzz_consumer LANGUAGES C CXX)

find_package(harfbuzz CONFIG REQUIRED)

add_executable(harfbuzz-consumer main.cpp)
target_link_libraries(harfbuzz-consumer PRIVATE harfbuzz::harfbuzz harfbuzz::harfbuzz-subset)
