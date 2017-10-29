cmake_minimum_required(VERSION 3.4)
project(file_monitor)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)

add_subdirectory(source)