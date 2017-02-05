cmake_minimum_required(VERSION 3.6)
project(Cpp_DistributedTwitter_Client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp server.cpp server.h)
add_executable(Cpp_DistributedTwitter_Client ${SOURCE_FILES})