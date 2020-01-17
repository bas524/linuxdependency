cmake_minimum_required(VERSION 3.5)

project(Qldd LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(Qldd
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  qldd.cpp
  qldd.h
)

target_link_libraries(Qldd PRIVATE Qt5::Widgets)
