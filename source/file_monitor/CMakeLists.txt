
set(TARGET_NAME file_monitor)

# For all platforms
set(SOURCE_LIST
  monitor.cpp
  monitor.hpp
  factory.cpp
  factory.hpp)
	
# Windows specific files
if(file_monitor_PLATFORM STREQUAL PLATFORM_WIN32)
	list(APPEND SOURCE_LIST
		win_monitor.cpp
		win_monitor.hpp)
endif()

# Mac OS X specific files
if(file_monitor_PLATFORM STREQUAL PLATFORM_MACOS)
	list(APPEND SOURCE_LIST
		mac_monitor.cpp
		mac_monitor.hpp)
endif()

# Linux specific files
if(file_monitor_PLATFORM STREQUAL PLATFORM_LINUX)
        list(APPEND SOURCE_LIST
                linux_monitor.cpp
                linux_monitor.hpp)
endif()

add_library(${TARGET_NAME}
  ${SOURCE_LIST})

target_link_libraries(${TARGET_NAME}
	PUBLIC Boost::filesystem Boost::disable_autolinking)

target_include_directories(${TARGET_NAME}
  INTERFACE ../)

target_compile_definitions(${TARGET_NAME}
	PRIVATE "-DFILE_MONITOR_${file_monitor_PLATFORM}")

install(TARGETS ${TARGET_NAME}
	ARCHIVE DESTINATION lib)

install(FILES
  factory.hpp
	monitor.hpp
	DESTINATION include/file_monitor)
