
set(TARGET_NAME file_monitor)
add_library(${TARGET_NAME}
  monitor.cpp
  monitor.hpp
  factory.cpp
  factory.hpp
  mac_monitor.cpp
  mac_monitor.hpp)


target_include_directories(${TARGET_NAME}
  PUBLIC ${Boost_INCLUDE_DIRS})