cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeWinIconv LANGUAGES C)

find_package(Iconv REQUIRED)

add_executable(win-iconv-consumer main.c)
target_link_libraries(win-iconv-consumer PRIVATE Iconv::Iconv)
