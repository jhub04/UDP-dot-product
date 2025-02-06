cmake_minimum_required(VERSION 3.29)
project(Nettverkprog_mappe)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread -Wall -Wextra")

if(APPLE)
    include_directories(/usr/local/include /opt/homebrew/include)
endif()

add_executable(Nettverkprog_mappe dot.cpp)
