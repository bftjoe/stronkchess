cmake_minimum_required(VERSION 2.8.9)
project (stronkchess)

include_directories(surge/src)
file(GLOB SOURCES "surge/src/*.cpp")

include_directories(cpp-httplib)

add_executable(stronkchess ${SOURCES} main.cpp)
