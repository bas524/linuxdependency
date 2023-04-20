if (APPLE)
    set(DEMANGLE_RULES_DEFAULT_PATH "Library/Application Support/qldd")
else ()
    set(DEMANGLE_RULES_DEFAULT_PATH ".config/qldd")
endif ()

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
