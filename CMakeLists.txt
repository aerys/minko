cmake_minimum_required(VERSION 3.5.1)

set(MINKO_HOME ${CMAKE_CURRENT_SOURCE_DIR})

include("minko.cmake")

add_flags()

#string(REPLACE "," ";" PLUGIN ${PLUGIN})
#message(${PLUGIN})

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif ()

string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

set (OUTPUT_PATH
    "${CMAKE_BINARY_DIR}/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_PATH})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_PATH})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH})

add_subdirectory("framework")

get_property(FRAMEWORK_INCLUDES TARGET minko-framework PROPERTY INCLUDE_DIRECTORIES)

include("plugins.cmake")

list (APPEND PLUGINS
    sdl
    png
)
foreach (PLUGIN IN LISTS PLUGINS)
    if (${PLUGIN})
        add_subdirectory("plugin/${PLUGIN}")
    endif ()
endforeach ()