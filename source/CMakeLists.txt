
set(TARGET_NAME file_monitor)

# For all platforms
set(SOURCE_LIST
  monitor.cpp
  monitor.hpp
  factory.cpp
  factory.hpp)
	
# Windows specific files
if(FILE_MONITOR_PLATFORM STREQUAL PLATFORM_WIN32)
	list(APPEND SOURCE_LIST
		win_monitor.cpp
		win_monitor.hpp)
endif()

# Mac OS X specific files
if(FILE_MONITOR_PLATFORM STREQUAL PLATFORM_MACOS)
	list(APPEND SOURCE_LIST
		mac_monitor.cpp
		mac_monitor.hpp)
endif()

# Linux specific files
if(FILE_MONITOR_PLATFORM STREQUAL PLATFORM_LINUX)
        list(APPEND SOURCE_LIST
                linux_monitor.cpp
                linux_monitor.hpp)
endif()

add_library(${TARGET_NAME}
  ${SOURCE_LIST})

target_include_directories(${TARGET_NAME}
  PUBLIC ${Boost_INCLUDE_DIRS})

target_compile_definitions(${TARGET_NAME}
	PRIVATE "-DFILE_MONITOR_${FILE_MONITOR_PLATFORM}")
