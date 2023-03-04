set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/modules")
cmake_minimum_required(VERSION 3.5)

set(MAJOR "1")
set(MINOR "1")
set(PATCH "1")
cmake_policy(SET CMP0048 NEW)

project(Qldd
    LANGUAGES CXX
    VERSION ${MAJOR}.${MINOR}.${PATCH})


find_package(VersionHeader)
set(PROJECT_VERSION "${MAJOR}.${MINOR}.${PATCH}.${COMMITTER_SHORTSHA}")

message(STATUS "COMMITTER_FULLSHA ${COMMITTER_FULLSHA}")
message(STATUS "COMMITTER_SHORTSHA ${COMMITTER_SHORTSHA}")
message(STATUS "COMMITTER_DATE ${COMMITTER_DATE}")
message(STATUS "PROJECT_VERSION ${PROJECT_VERSION}")

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
include_directories(${CMAKE_BINARY_DIR})
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
target_compile_definitions(Qldd PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(Qldd PRIVATE ${QtVer}::Widgets)

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
                          MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} Copyright (c) 2023 bas524@yandex.ru"
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

# Components:
if(CMAKE_BUILD_TYPE_UPPER MATCHES "^(DEBUG|RELWITHDEBINFO)$")
    set(CPACK_STRIP_FILES FALSE)
else()
    set(CPACK_STRIP_FILES TRUE)
endif()

install(TARGETS Qldd RUNTIME DESTINATION bin COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/resources/qldd.desktop DESTINATION share/applications/ COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/resources/Qldd.png DESTINATION share/pixmaps/ COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/nemo/actions/dependency-viewer.nemo_action
        DESTINATION share/nemo/actions
        COMPONENT nemo-integration)
install(FILES ${CMAKE_SOURCE_DIR}/nautilus-python/extensions/dependency-viewer.py
        DESTINATION nautilus-python/extensions
        COMPONENT nautilus-integration)

set(CPACK_SOURCE_IGNORE_FILES
    #svn files
    "\\\\.svn/"
    "\\\\.cvsignore$"
    #git files
    "\\\\.git/"
    "\\\\.github/"
    # temporary files
    "\\\\.swp$"
    # backup files
    "~$"
    # eclipse files
    "\\\\.cdtproject$"
    "\\\\.cproject$"
    "\\\\.project$"
    "\\\\.settings/"
    # others
    "\\\\.#"
    "/#"
    "/build/"
    "/_build/"
    "/\\\\.git/"
    # used before
    "/CVS/"
    "/\\\\.libs/"
    "/\\\\.deps/"
    "\\\\.o$"
    "\\\\.lo$"
    "\\\\.la$"
    "Makefile\\\\.in$"
    )


set(CPACK_PACKAGE_NAME "Qldd")
set(CPACK_PACKAGE_VERSION "${MAJOR}.${MINOR}.${PATCH}")
set(CPACK_PACKAGE_VENDOR "community")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/bas524/linuxdependency")
set(CPACK_PACKAGE_CONTACT "bas524")
set(CPACK_PACKAGE_DESCRIPTION "DependencyViewer shows all dependecies of a given executable or dynamic library. It is a GUI wrapper for the ldd, file and nm commands")

set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CMAKE_PROJECT_HOMEPAGE_URL})
set(CPACK_DEBIAN_PACKAGE_LICENSE "Apache-2.0")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "python3-nautilus, binutils, libqt5core5a, libqt5gui5, libqt5widgets5")

set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_RPM_PACKAGE_SECTION "net")
set(CPACK_RPM_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_URL ${CMAKE_PROJECT_HOMEPAGE_URL})
set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
set(CPACK_RPM_PACKAGE_REQUIRES "python3-nautilus, binutils, qt5-qtbase")

include(CPack)
cpack_add_component(application DISPLAY_NAME "DependencyViewer" REQUIRED)

