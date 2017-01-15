cmake_minimum_required(VERSION 3.6)
project(Cpp_DistributedTwitter_Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(Cpp_DistributedTwitter_Server ${SOURCE_FILES})
target_link_libraries(Cpp_DistributedTwitter_Server pthread)