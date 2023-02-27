cmake_minimum_required(VERSION 3.5)

project(Qldd
    LANGUAGES CXX
    VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QtVer Qt5)
find_package(${QtVer} COMPONENTS Widgets QUIET)
if (NOT Qt5_FOUND)
  set(QtVer Qt6)
  find_package(${QtVer} COMPONENTS Widgets REQUIRED)
endif ()

if (APPLE)
  set(APP_ICNS "${CMAKE_SOURCE_DIR}/resources/Qldd.icns")
  add_executable(Qldd MACOSX_BUNDLE ${APP_ICNS})
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
    resources/resources.qrc
    )
target_sources(Qldd PRIVATE ${SOURCES})

target_link_libraries(Qldd PRIVATE ${QtVer}::Widgets)

install(TARGETS Qldd
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        COMPONENT DependencyViewer)

install(FILES ${CMAKE_SOURCE_DIR}/nemo/actions/dependency-viewer.nemo_action
        DESTINATION share/nemo/actions
        COMPONENT nemo-integration)

if(APPLE)
    set(CMAKE_MACOSX_BUNDLE ON)
    set(CMAKE_MACOSX_RPATH ON)
    install(TARGETS Qldd
            BUNDLE DESTINATION .
            RUNTIME DESTINATION .)
    set_source_files_properties("${CMAKE_SOURCE_DIR}/resources/Qldd.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set_target_properties(Qldd
                          PROPERTIES
                          MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
                          MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} Copyright (c) 2020 bas524@yandex.ru"
                          MACOSX_BUNDLE_ICON_FILE Qldd.icns
                          MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}"
                          MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
                          MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
                          MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
                          MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2020 bas524@yandex.ru"
                          )
    if (${QtVer} STREQUAL Qt5)
      add_custom_command(TARGET Qldd
                        POST_BUILD
                        COMMAND plutil -replace NSHighResolutionCapable -bool true Qldd.app/Contents/Info.plist
                        )
      endif()
endif()

set(desktop.path applications)
set(desktop.files resources/qldd.desktop)
set(icon.path icons/hicolor/64x64/apps)
set(icon.files resources/Qldd.png)
set(iconsvg.path icons/hicolor/scalable/apps)
set(iconsvg.files resources/Qldd.svg)
foreach(items IN ITEMS desktop icon iconsvg)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${${items}.files}
            DESTINATION share/${${items}.path}
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
endforeach()
# Components:
if(CMAKE_BUILD_TYPE_UPPER MATCHES "^(DEBUG|RELWITHDEBINFO)$")
    set(CPACK_STRIP_FILES FALSE)
else()
    set(CPACK_STRIP_FILES TRUE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/Packaging.cmake)

