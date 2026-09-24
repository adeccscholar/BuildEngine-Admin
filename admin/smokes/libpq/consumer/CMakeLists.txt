cmake_minimum_required(VERSION 3.25)

project(libpq_consumer LANGUAGES C)

find_package(PostgreSQL REQUIRED)

add_executable(libpq-consumer main.c)
target_link_libraries(libpq-consumer PRIVATE PostgreSQL::PostgreSQL)
