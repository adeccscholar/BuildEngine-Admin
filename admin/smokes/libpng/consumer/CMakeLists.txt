cmake_minimum_required(VERSION 3.20)
project(libpng_consumer LANGUAGES C)

find_package(PNG REQUIRED)

add_executable(libpng-consumer main.c)
target_link_libraries(libpng-consumer PRIVATE PNG::PNG)
