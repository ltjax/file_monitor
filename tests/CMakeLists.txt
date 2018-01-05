
set(TARGET_NAME	tests)

add_executable(${TARGET_NAME} EXCLUDE_FROM_ALL
	scoped_temp_folder.cpp
	scoped_temp_folder.hpp
	integration.cpp
  main.cpp)
  
target_link_libraries(${TARGET_NAME}
  file_monitor)

target_include_directories(${TARGET_NAME}
  PUBLIC .)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
