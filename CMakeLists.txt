cmake_minimum_required(VERSION 3.22)

project(MUSHpad VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(MUSHpad
    mainwindow.ui
    mainwindow.cpp
    main.cpp
)

target_link_libraries(MUSHpad PRIVATE Qt6::Widgets)

set_target_properties(MUSHpad PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)