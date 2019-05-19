cmake_minimum_required(VERSION 3.4)
project(file_monitor)

set(CMAKE_CXX_STANDARD 14)

option(file_monitor_BUILD_TESTS "Build tests" ON)
option(file_monitor_FIND_BOOST "Find boost library via find_package" ON)
option(file_monitor_USE_CONAN "Use conan" ON)

# Canonize platform selection
set(file_monitor_PLATFORM Undefined)
if(WIN32)
  set(file_monitor_PLATFORM PLATFORM_WIN32)
elseif(APPLE)
  set(file_monitor_PLATFORM PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
  set(file_monitor_PLATFORM PLATFORM_LINUX)
endif()

if (file_monitor_USE_CONAN)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
elseif(file_monitor_FIND_BOOST)
  find_package(Boost REQUIRED COMPONENTS filesystem)
  set(BOOST_IMPORT_TARGETS Boost::filesystem Boost::disable_autolinking)
endif()

# Go for the library
add_subdirectory(source)

if(file_monitor_BUILD_TESTS)
  # Go for the tests
  add_subdirectory(tests)
endif()
