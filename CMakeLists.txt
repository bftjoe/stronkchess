cmake_minimum_required(VERSION 2.8.9)
project (stronkchess)

include_directories(surge/src)

file(GLOB SOURCES "surge/src/*.cpp")

add_executable(stronkchess ${SOURCES} main.cpp)