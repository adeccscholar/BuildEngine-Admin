cmake_minimum_required(VERSION 3.25)

project(BuildEngineSmokeExpat LANGUAGES C)

find_package(expat 2.8.4 CONFIG REQUIRED char dtd ns)

add_executable(expat-consumer main.c)
target_link_libraries(expat-consumer PRIVATE expat::expat)
