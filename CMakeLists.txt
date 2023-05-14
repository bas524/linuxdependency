set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/modules")
cmake_minimum_required(VERSION 3.5)

set(MAJOR "1")
set(MINOR "2")
set(PATCH "1")
cmake_policy(SET CMP0048 NEW)

project(Qldd
    LANGUAGES CXX
    VERSION ${MAJOR}.${MINOR}.${PATCH})


find_package(VersionHeader)
set(PROJECT_VERSION "${MAJOR}.${MINOR}.${PATCH}.${COMMITTER_SHORTSHA}")
file(WRITE ${CMAKE_SOURCE_DIR}/VERSION.txt "${MAJOR}.${MINOR}.${PATCH}")
find_package(ConfigPath)
configure_file(${CMAKE_SOURCE_DIR}/resources/qldd.desktop.in ${CMAKE_BINARY_DIR}/resources/qldd.desktop @ONLY)

function(get_linux_lsb_release_information)
    find_program(LSB_RELEASE_EXEC lsb_release)
    if(NOT LSB_RELEASE_EXEC)
        message(FATAL_ERROR "Could not detect lsb_release executable, can not gather required information")
    endif()

    execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --id OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --release OUTPUT_VARIABLE LSB_RELEASE_VERSION_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --codename OUTPUT_VARIABLE LSB_RELEASE_CODENAME_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(LSB_RELEASE_ID_SHORT "${LSB_RELEASE_ID_SHORT}" PARENT_SCOPE)
    set(LSB_RELEASE_VERSION_SHORT "${LSB_RELEASE_VERSION_SHORT}" PARENT_SCOPE)
    set(LSB_RELEASE_CODENAME_SHORT "${LSB_RELEASE_CODENAME_SHORT}" PARENT_SCOPE)
endfunction()

message(STATUS "COMMITTER_FULLSHA ${COMMITTER_FULLSHA}")
message(STATUS "COMMITTER_SHORTSHA ${COMMITTER_SHORTSHA}")
message(STATUS "COMMITTER_DATE ${COMMITTER_DATE}")
message(STATUS "PROJECT_VERSION ${PROJECT_VERSION}")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    get_linux_lsb_release_information()
    message(STATUS "OS Linux ${LSB_RELEASE_ID_SHORT} ${LSB_RELEASE_VERSION_SHORT} ${LSB_RELEASE_CODENAME_SHORT}")
else()
    message(STATUS "OS ${CMAKE_SYSTEM_NAME}")
    message(STATUS "OS VERSION ${CMAKE_SYSTEM_VERSION}")
endif()

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
    demanglerules.h
    demanglerules.cpp
    demanglerules.ui
    customtypes.h
    resources/resources.qrc
    )
target_sources(Qldd PRIVATE ${SOURCES})
target_compile_definitions(Qldd PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}" -DQ_COMPILER_INITIALIZER_LISTS)
target_link_libraries(Qldd PRIVATE ${QtVer}::Widgets)

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

if(APPLE)
    set(CMAKE_MACOSX_BUNDLE ON)
    set(CMAKE_MACOSX_RPATH ON)
    set(CPACK_DMG_BACKGROUND_IMAGE
          "${CMAKE_SOURCE_DIR}/resources/CMakeDMGBackground.tif")
      set(CPACK_DMG_DS_STORE_SETUP_SCRIPT
          "${CMAKE_SOURCE_DIR}/resources/CMakeDMGSetup.scpt")

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
                          MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2023 bas524@yandex.ru"
                          )
    if (${QtVer} STREQUAL Qt5)
      add_custom_command(TARGET Qldd
                        POST_BUILD
                        COMMAND plutil -replace NSHighResolutionCapable -bool true Qldd.app/Contents/Info.plist
                        )
      endif()
else()

    # Components:
    if(CMAKE_BUILD_TYPE_UPPER MATCHES "^(DEBUG|RELWITHDEBINFO)$")
        set(CPACK_STRIP_FILES FALSE)
    else()
        set(CPACK_STRIP_FILES TRUE)
    endif()

    install(TARGETS Qldd RUNTIME DESTINATION bin
                         BUNDLE DESTINATION bin
                         COMPONENT application)
    install(FILES ${CMAKE_BINARY_DIR}/resources/qldd.desktop DESTINATION share/applications/ COMPONENT application)
    install(FILES ${CMAKE_SOURCE_DIR}/resources/Qldd.png DESTINATION share/pixmaps/ COMPONENT application)
    install(FILES ${CMAKE_SOURCE_DIR}/nemo/actions/dependency-viewer.nemo_action
            DESTINATION share/nemo/actions
            COMPONENT nemo-integration)
    install(FILES ${CMAKE_SOURCE_DIR}/nautilus-python/extensions/dependency-viewer.py
            DESTINATION share/nautilus-python/extensions
            COMPONENT nautilus-integration)
    install(FILES ${CMAKE_SOURCE_DIR}/nautilus-python/extensions/dependency-viewer-43.py
            DESTINATION share/nautilus-python/extensions
            COMPONENT nautilus-integration)
    install(FILES ${CMAKE_SOURCE_DIR}/dolphin/kde-qldd-menu.desktop
            DESTINATION share/kservices5/ServiceMenus
            COMPONENT dolphin-integration)

    set(PYTHON_NAUTILUS_DEB "python-nautilus")
    set(PYTHON_NAUTILUS_RPM "nautilus-python")
    if (LSB_RELEASE_ID_SHORT MATCHES "Ubuntu" AND LSB_RELEASE_CODENAME_SHORT MATCHES "jammy")
        set(PYTHON_NAUTILUS_DEB "python3-nautilus")
    endif()
    if (LSB_RELEASE_ID_SHORT MATCHES "Debian" AND LSB_RELEASE_CODENAME_SHORT MATCHES "bullseye")
        set(PYTHON_NAUTILUS_DEB "python3-nautilus")
    endif()
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
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${PYTHON_NAUTILUS_DEB}, binutils, libqt5core5a, libqt5gui5, libqt5widgets5")
    SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/resources/postinst")

    set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
    set(CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
    set(CPACK_RPM_PACKAGE_SECTION "net")
    set(CPACK_RPM_PACKAGE_MAINTAINER ${CPACK_PACKAGE_MAINTAINER})
    set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
    set(CPACK_RPM_PACKAGE_URL ${CMAKE_PROJECT_HOMEPAGE_URL})
    set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
    set(CPACK_RPM_PACKAGE_REQUIRES "${PYTHON_NAUTILUS_RPM}, binutils, qt5-qtbase")
    SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/resources/postinst")
endif()
include(CPack)
cpack_add_component(application DISPLAY_NAME "DependencyViewer" REQUIRED)

