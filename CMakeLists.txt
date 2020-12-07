cmake_minimum_required(VERSION 3.5.1)

set(MINKO_HOME ${CMAKE_CURRENT_BINARY_DIR})

option (WITH_PLUGINS "list off plugins to build" ON)
option (WITH_EXAMPLES "list off examples to build" ON)
option (WITH_TESTS "used to enable the tests" OFF)
option (WITH_WASM "enable WebAssembly" ON)
option (WITH_OFFSCREEN "enable offscreen rendering" OFF)
option (WITH_NODEJS_WORKER "enable the NodeJS worker plugin" OFF)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif ()

file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/template" DESTINATION ${MINKO_HOME})
file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/cmake" DESTINATION ${MINKO_HOME})
include ("${MINKO_HOME}/cmake/project.cmake")
include ("${MINKO_HOME}/cmake/plugins.cmake")
include ("${MINKO_HOME}/cmake/package.cmake")
include ("${MINKO_HOME}/cmake/minko.cmake")
include ("${MINKO_HOME}/cmake/android-build.cmake")

if (WITH_OFFSCREEN)
    set (OFFSCREEN_PATH plugin/offscreen)
    file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/${OFFSCREEN_PATH}/enable.cmake DESTINATION ${MINKO_HOME}/${OFFSCREEN_PATH})
    file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/${OFFSCREEN_PATH}/include DESTINATION ${MINKO_HOME}/${OFFSCREEN_PATH})
    file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/${OFFSCREEN_PATH}/lib DESTINATION ${MINKO_HOME}/${OFFSCREEN_PATH})
    include ("${MINKO_HOME}/../plugin/offscreen/enable.cmake")
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "_UNSET" CACHE STRING "")
if(${CMAKE_CXX_FLAGS_DEBUG} STREQUAL "_UNSET")
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0" CACHE STRING "" FORCE)
endif()

if (APPLE AND NOT IOS)
   set (CMAKE_OSX_DEPLOYMENT_TARGET "")
   set (CMAKE_OSX_SYSROOT /)
   set (CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.7)
   set (CMAKE_OSX_SYSROOT:STRING=macosx10.11)
endif ()

add_subdirectory ("framework")
include("framework/enable.cmake")

list (
    APPEND
    PLUGINS
    sdl
    png
    zlib
    jpeg
    ttf
    serializer
    fx
    assimp
    devil
    ssl
    debug
    http-worker
    http-loader
    html-overlay
    bullet
    angle
    sensors
    video-camera
    vr
    websocket
)

if (WITH_NODEJS_WORKER)
    if (NOT WITH_PLUGINS MATCHES OFF AND NOT WITH_PLUGINS MATCHES ON)
        set (WITH_PLUGINS "nodejs-worker,${WITH_PLUGINS}")
    endif ()
    list (APPEND PLUGINS nodejs-worker)
endif ()

if (WITH_OFFSCREEN)
    if (NOT WITH_PLUGINS MATCHES OFF AND NOT WITH_PLUGINS MATCHES ON)
        set (WITH_PLUGINS "offscreen,${WITH_PLUGINS}")
    endif ()
    list (APPEND PLUGINS offscreen)
endif ()

list (
    APPEND
    EXAMPLES
    cube
    assimp
    benchmark-cube
    blending
    devil
    html-overlay
    http #emscripten compil error #(can't find BaseStreamingTemplate.effect)
    joystick
    #keyboard #(-lgl flag should be at the the end of the linkage line)
    light #(light display is under the ground)
    line-geometry #can't see the stars
    pbr #error execution (error: DrawCall.cpp:305 bindUniform(): Program "skybox-pass0": the uniform "uAspectRatio" is bound to the "aspectRatio" property but it's not defined and no default value was provided.)
    physics
    sensors #segfault
    serializer
    shadow-mapping
    sky-box
    sprite-batch
    stencil
    #ttf #emscripten and apple compilation error
    #visibility
    #vr #(symbol missing in vr library)
)

if (WITH_OFFSCREEN)
    list (APPEND EXAMPLES offscreen)
endif ()

if (IOS)
    unset(CMAKE_OSX_DEPLOYMENT_TARGET CACHE)
    set (CMAKE_OSX_SYSROOT iphoneos)
    set (CMAKE_OSX_ARCHITECTURES "armv7")
    set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set (CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif ()

# -DWITH_PLUGINS flag management
if (NOT WITH_PLUGINS MATCHES OFF)
    string (TOLOWER "${WITH_PLUGINS}" WITH_PLUGINS_LOWER)
    string(FIND ${WITH_PLUGINS_LOWER} "," TEST_COMMA)
    # Test if there is plugins names speficied
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
    # Test if all plugins are activate
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

# -DWITH_EXAMPLES flag management
if (NOT WITH_EXAMPLES MATCHES OFF)
    string (TOLOWER "${WITH_EXAMPLES}" WITH_EXAMPLES_LOWER)
    string(FIND ${WITH_EXAMPLES_LOWER} "," TEST_COMMA)
    # Test if there is examples names speficied
    if (NOT ${TEST_COMMA} EQUAL -1)
        string (REPLACE "," ";" USER_EXAMPLES ${WITH_EXAMPLES_LOWER})
        foreach (USER_EXAMPLE ${USER_EXAMPLES})
            foreach (EXAMPLE ${EXAMPLES})
                if (${USER_EXAMPLE} STREQUAL ${EXAMPLE})
                    add_subdirectory("example/${EXAMPLE}")
                endif ()
            endforeach()
        endforeach ()
    # Test if all examples are activate
    elseif (WITH_EXAMPLES MATCHES ON)
        foreach (EXAMPLE ${EXAMPLES})
            add_subdirectory("example/${EXAMPLE}")
        endforeach ()
    else ()
        add_subdirectory("example/${WITH_EXAMPLES}")
    endif ()
endif ()

if (WITH_TESTS)
    add_subdirectory ("test")
endif ()

