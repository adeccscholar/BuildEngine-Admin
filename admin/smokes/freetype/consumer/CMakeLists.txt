cmake_minimum_required(VERSION 3.20)
project(freetype_consumer LANGUAGES C)

find_package(Freetype REQUIRED)

add_executable(freetype-consumer main.c)
target_link_libraries(freetype-consumer PRIVATE Freetype::Freetype)
