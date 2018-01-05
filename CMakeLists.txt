cmake_minimum_required(VERSION 3.4)
project(file_monitor)

set(CMAKE_CXX_STANDARD 14)

option(file_monitor_BUILD_TESTS "Build tests" ON)

# Canonize platform selection
set(file_monitor_PLATFORM Undefined)
if(WIN32)
  set(file_monitor_PLATFORM PLATFORM_WIN32)
elseif(APPLE)
  set(file_monitor_PLATFORM PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
  set(file_monitor_PLATFORM PLATFORM_LINUX)
endif()


# Dependencies
# TODO: Need to verify that requiring COMPONENTS works for all platforms
#       and all targeted boost releases. Currently, there seems to be some
#       trouble requiring components on OSX.
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED COMPONENTS filesystem)
endif()

# Go for the library
add_subdirectory(source)

if(file_monitor_BUILD_TESTS)
  # Go for the tests
  add_subdirectory(tests)
endif()
