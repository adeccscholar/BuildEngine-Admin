cmake_minimum_required(VERSION 3.20)
project(libtiff_consumer LANGUAGES C)

find_package(TIFF REQUIRED)

add_executable(libtiff-consumer main.c)
target_link_libraries(libtiff-consumer PRIVATE TIFF::TIFF)
