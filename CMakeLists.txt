cmake_minimum_required(VERSION 3.5.1)

set(MINKO_HOME ${CMAKE_CURRENT_SOURCE_DIR})

include ("plugins.cmake")
include ("package.cmake")
include ("minko.cmake")
include ("project.cmake")
include ("copy.cmake")

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif ()

add_subdirectory("framework")

get_property (FRAMEWORK_INCLUDES TARGET minko-framework PROPERTY INCLUDE_DIRECTORIES)

option (WITH_PLUGINS "list off plugins to build" ON)

list (APPEND PLUGINS
    sdl
    png
    ttf
    serializer
    fx
)

unset(CMAKE_OSX_DEPLOYMENT_TARGET CACHE)
if (IOS)
    set (CMAKE_OSX_SYSROOT iphoneos)
    set (CMAKE_OSX_ARCHITECTURES "armv7")
    set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set (CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif ()

if (NOT WITH_PLUGINS MATCHES OFF)
    string (TOLOWER "${WITH_PLUGINS}" WITH_PLUGINS_LOWER)
    string(FIND ${WITH_PLUGINS_LOWER} "," TEST_COMMA)
    if (NOT ${TEST_COMMA} EQUAL -1)
        string (REPLACE "," ";" USER_PLUGINS ${WITH_PLUGINS_LOWER})
        foreach (USER_PLUGIN ${USER_PLUGINS})
            foreach (PLUGIN ${PLUGINS})
                if (${USER_PLUGIN} STREQUAL ${PLUGIN})
                    add_subdirectory("plugin/${PLUGIN}")
                    include("plugin/${PLUGIN}/enable.cmake")
                endif ()
            endforeach()
        endforeach ()
    elseif (WITH_PLUGINS MATCHES ON)
        foreach (PLUGIN ${PLUGINS})
            add_subdirectory("plugin/${PLUGIN}")
            include("plugin/${PLUGIN}/enable.cmake")
        endforeach ()
    else ()
        add_subdirectory("plugin/${WITH_PLUGINS}")
        include("plugin/${WITH_PLUGINS}/enable.cmake")
    endif ()
endif ()

add_subdirectory("example/cube")
