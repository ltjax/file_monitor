
set(TARGET_NAME file_monitor)
add_library(${TARGET_NAME}
        ChangeList.cpp
        ChangeList.hpp
        ChangeListFactory.cpp
        ChangeListFactory.hpp
        MacChangeList.cpp
        MacChangeList.hpp)
