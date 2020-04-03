
set(TARGET_NAME file_monitor)

# For all platforms
set(SOURCE_LIST
  monitor.cpp
  monitor.hpp
  debouncer.cpp
  debouncer.hpp
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

if(file_monitor_USE_BOOST)
  target_link_libraries(${TARGET_NAME}
    PUBLIC ${BOOST_TARGETS})
  target_compile_definitions(${TARGET_NAME}
    PUBLIC "file_monitor_USE_BOOST")
endif()

# Need to link to stdc++fs for g++8, or using <filesystem> will crash
if(file_monitor_USE_CXX17)
  target_link_libraries(${TARGET_NAME}
    PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)
endif()

if(file_monitor_PLATFORM STREQUAL PLATFORM_MACOS)
  target_link_libraries(${TARGET_NAME}
    PUBLIC "-framework CoreFoundation"
    PUBLIC "-framework CoreServices")
endif()

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
