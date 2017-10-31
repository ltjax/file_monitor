cmake_minimum_required(VERSION 3.4)
project(file_monitor)

set(CMAKE_CXX_STANDARD 14)

# Canonize platform selection
set(FILE_MONITOR_PLATFORM Undefined)
if(WIN32)
  set(FILE_MONITOR_PLATFORM PLATFORM_WIN32)
elseif(APPLE)
	set(FILE_MONITOR_PLATFORM PLATFORM_MACOS)
endif()

# Dependencies
find_package(Boost REQUIRED)

# Go for the library
add_subdirectory(source)