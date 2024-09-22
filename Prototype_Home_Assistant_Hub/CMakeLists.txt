cmake_minimum_required(VERSION 3.5)

project(Prototype_Home_Assistant_Hub)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(PROJECT_SOURCES
        include/mainwindow.h
        include/resizableframe.h
        include/gridItemMover/gridItemMover.h
        
        src/main.cpp
        src/mainwindow.cpp
        src/resizableframe.cpp
        src/gridItemMover/gridItemMover.cpp
        
        ui/prototype_home_assistant_hub.ui
)

find_package(Qt5Widgets REQUIRED)

add_executable(Prototype_Home_Assistant_Hub ${PROJECT_SOURCES})

target_link_libraries(Prototype_Home_Assistant_Hub Qt5::Widgets)