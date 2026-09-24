cmake_minimum_required(VERSION 3.25)

project(libpqxx_consumer LANGUAGES CXX)

find_package(libpqxx CONFIG REQUIRED)

add_executable(libpqxx-consumer main.cpp)
target_compile_features(libpqxx-consumer PRIVATE cxx_std_23)
target_link_libraries(libpqxx-consumer PRIVATE libpqxx::pqxx)
