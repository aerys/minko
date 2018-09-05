cmake_minimum_required(VERSION 3.5.1)

set(MINKO_HOME ${CMAKE_CURRENT_SOURCE_DIR})

include ("plugins.cmake")
include ("package.cmake")
include ("minko.cmake")
include ("project.cmake")
include ("copy.cmake")
include ("android-build.cmake")

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif ()

if (APPLE AND NOT IOS)
   set (CMAKE_OSX_DEPLOYMENT_TARGET "")
   set (CMAKE_OSX_SYSROOT /)
   set (CMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.7)
   set (CMAKE_OSX_SYSROOT:STRING=macosx10.11)
endif ()

add_subdirectory("framework")

get_property (FRAMEWORK_INCLUDES TARGET minko-framework PROPERTY INCLUDE_DIRECTORIES)

option (WITH_PLUGINS "list off plugins to build" ON)
option (WITH_EXAMPLES "list off examples to build" ON)

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
    html-overlay #apple compil error
    #leap (out dated plugin - math dependencies)
    offscreen
    #lua (out dated plugin)
    #particles (out dated plugin - lua dependencies)
    bullet
    angle
    sensors
    video-camera
    vr
    websocket
    nodejs-worker
)

list (
    APPEND
    EXAMPLES
    cube
    serializer
    assimp
    benchmark-cube
    blending
    cel-shading
    fog
    audio
    #clone #(error lookAt segfault if did like in the serializer)
    #hologram
    devil
    #effect-config #(error surface when resolve vector 4 texture don't provide 'default')
    #flares
    #frustum
    #html-overlay #(plugin html-overlay problem)
    http #EMSCRIPTEN COMPIL ERROR #(don't find BaseStreamingTemplate.effect but he is in asset)
    jobs
    joystick
    #keyboard #(-lgl shoudl be at the the of the compil line)
    #leap-motion #(see plugin leap)
    light # (display of light is behind ground)
    #light-scattering #(html_overlay)
    line-geometry #work but dont see stars
    #multi-surfaces
    #nodejs #(we should make it too ?)
    #oculus #(oculus plugin don't exist anymore)
    #offscreen #(canvas->quit() n'existe plus)
    #particles #(cause of lua)
    pbr #error execution (error: DrawCall.cpp:305 bindUniform(): Program "skybox-pass0": the uniform "uAspectRatio" is bound to the "aspectRatio" property but it's not defined and no default value was provided.)
    physics
    picking
    #raycasting
    section #ems compil error
    sensors #segfault
    shadow-mapping
    sky-box
    sprite-batch
    stencil
    #ttf #ems and apple compil error
    video-camera
    #visibility #(cpp errors with visility)
    #vr #(symbol missing in vr lib)
    #water #undefined symbol
)

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