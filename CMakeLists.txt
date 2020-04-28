cmake_minimum_required(VERSION 3.5)

project(Qldd LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if (APPLE)
  set(MACOSX_BUNDLE_BUNDLE_NAME Qldd)
  set(BUNDLE_VAL MACOSX_BUNDLE)
  add_executable(Qldd MACOSX_BUNDLE)
else ()
  add_executable(Qldd)
endif ()

set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    qldd.cpp
    qldd.h
    )
target_sources(Qldd PRIVATE ${SOURCES})
target_link_libraries(Qldd PRIVATE Qt5::Widgets)

install(TARGETS Qldd
        RUNTIME  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        BUNDLE  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        COMPONENT DependencyViewer)

install(FILES ${CMAKE_SOURCE_DIR}/nemo/actions/dependency-viewer.nemo_action
        DESTINATION share/nemo/actions
        COMPONENT nemo-integration)
