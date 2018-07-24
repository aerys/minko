cmake_minimum_required(VERSION 3.5.1)

set(MINKO_HOME ${CMAKE_CURRENT_SOURCE_DIR})

include("minko.cmake")

add_flags()

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif ()

string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

add_subdirectory("framework")

get_property (FRAMEWORK_INCLUDES TARGET minko-framework PROPERTY INCLUDE_DIRECTORIES)

include ("plugins.cmake")
include ("package.cmake")

if (WITH_PLUGINS)
    string (TOLOWER "${WITH_PLUGINS}" WITH_PLUGINS_LOWER)
    string(FIND ${WITH_PLUGINS_LOWER} "," TEST_COMMA)
    if (TEST_COMMA)
        string (REPLACE "," ";" USER_PLUGINS ${WITH_PLUGINS_LOWER})
    endif ()
endif ()

list (APPEND PLUGINS
    sdl
    png
)

#if (USER_PLUGINS)
#    foreach (USER_PLUGIN IN_LISTS USER_PLUGINS)
#        foreach (PLUGIN IN LISTS PLUGINS)
#            if (${USER_PLUGIN} STREQUAL ${PLUGIN})
#                add_subdirectory("plugin/${PLUGIN}")
#                include("plugin/${PLUGIN}/enable.cmake")
                #get_property (${PLUGIN}_INCLUDES TARGET minko-plugin-${PLUGIN} PROPERTY INCLUDE_DIRECTORIES)
#            endif ()
#        endforeach()
#    endforeach ()
#elseif (${WITH_PLUGINS} STREQUAL "on")
#    foreach (PLUGIN IN_LISTS PLUGINS)
#        add_subdirectory("plugin/${PLUGIN}")
#        include("plugin/${PLUGIN}/enable.cmake")
#    endforeach ()
#endif ()

add_subdirectory("plugin/sdl")
add_subdirectory("plugin/png")
include("plugin/sdl/enable.cmake")
include("plugin/png/enable.cmake")

add_subdirectory("example/cube")