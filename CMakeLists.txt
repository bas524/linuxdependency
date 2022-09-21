cmake_minimum_required(VERSION 3.5)

project(Qldd LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Qt5_in_use true)
find_package(Qt5 COMPONENTS Widgets QUIET)
if (NOT Qt5_FOUND)
  set(Qt5_in_use false)
  find_package(Qt6 COMPONENTS Widgets REQUIRED)
endif ()

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
    finfdialog.cpp
    finfdialog.h
    finfdialog.ui
    )
target_sources(Qldd PRIVATE ${SOURCES})
if (Qt5_in_use)
  target_link_libraries(Qldd PRIVATE Qt5::Widgets)
else ()
  target_link_libraries(Qldd PRIVATE Qt6::Widgets)
endif ()
install(TARGETS Qldd
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        COMPONENT DependencyViewer)

install(FILES ${CMAKE_SOURCE_DIR}/nemo/actions/dependency-viewer.nemo_action
        DESTINATION share/nemo/actions
        COMPONENT nemo-integration)
