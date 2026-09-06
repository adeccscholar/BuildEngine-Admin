cmake_minimum_required(VERSION 3.20)
project(libjpeg_turbo_consumer LANGUAGES C)

find_package(JPEG REQUIRED)

add_executable(libjpeg-turbo-consumer main.c)
target_link_libraries(libjpeg-turbo-consumer PRIVATE JPEG::JPEG)
