cmake_minimum_required(VERSION 3.4)
project(file_monitor)

set(CMAKE_CXX_STANDARD 14)

# Canonize platform selection
set(FILE_MONITOR_PLATFORM Undefined)
if(WIN32)
  set(FILE_MONITOR_PLATFORM PLATFORM_WIN32)
elseif(APPLE)
  set(FILE_MONITOR_PLATFORM PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
  set(FILE_MONITOR_PLATFORM PLATFORM_LINUX)
endif()

# Dependencies
# TODO: Need to verify that requiring COMPONENTS works for all platforms
#       and all targeted boost releases. Currently, there seems to be some
#       trouble requiring components on OSX.
find_package(Boost REQUIRED)

# Go for the library
add_subdirectory(source)
