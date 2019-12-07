cmake_minimum_required(VERSION 3.4)
project(file_monitor)


option(file_monitor_BUILD_TESTS "Build tests" ON)
option(file_monitor_FIND_BOOST "Find boost library via find_package" OFF)
option(file_monitor_USE_CONAN "Use conan" ON)
option(file_monitor_USE_BOOST "Use boost instead of C++17 features" OFF)
option(file_monitor_USE_CXX17 "Use C++17 features" ON)

# Validate options
if (NOT file_monitor_USE_BOOST AND NOT file_monitor_USE_CXX17)
  message(FATAL_ERROR "Need to enable either boost or C++17 for filesystem types")
endif()

if (file_monitor_FIND_BOOST AND NOT file_monitor_USE_BOOST)
  message(FATAL_ERROR "No need to find boost if not using boost")
endif()

# Optionally enable C++17
if (file_monitor_USE_CXX17)
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()

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
endif()

# Handle Boost dependency
if (file_monitor_USE_BOOST)
  if (file_monitor_USE_CONAN)
    set(BOOST_TARGETS
      CONAN_PKG::boost_filesystem
      CONAN_PKG::boost_algorithm
      CONAN_PKG::boost_range)

    if(file_monitor_PLATFORM STREQUAL PLATFORM_MACOS)
      set(BOOST_TARGETS ${BOOST_TARGETS}
        PUBLIC CONAN_PKG::boost_iostreams)
    endif()
  elseif(file_monitor_FIND_BOOST)
    find_package(Boost REQUIRED COMPONENTS filesystem)
    set(BOOST_TARGETS Boost::filesystem Boost::disable_autolinking)
  endif()
endif()

# Go for the library
add_subdirectory(source)

if(file_monitor_BUILD_TESTS)
  message("Building tests..")
  # Go for the tests
  add_subdirectory(tests)
endif()
