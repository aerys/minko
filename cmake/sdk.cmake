include ("${MINKO_HOME}/cmake/plugins.cmake")
include ("${MINKO_HOME}/cmake/package.cmake")
include ("${MINKO_HOME}/cmake/minko.cmake")
include ("${MINKO_HOME}/cmake/project.cmake")
include ("${MINKO_HOME}/cmake/android-build.cmake")

list (
    APPEND
    PLUGINS
    zlib
    assimp
    debug
    devil
    bullet
    fx
    html-overlay
    http-loader
    http-worker
    jpeg
    png
    sdl
    ssl
    sensors
    serializer
    ttf
    video-camera
    vr
    websocket
)

if (WITH_NODEJS_WORKER STREQUAL "ON" OR WITH_NODEJS_WORKER STREQUAL "ON")
    list (APPEND PLUGINS nodejs-worker)
endif ()

if (WITH_OFFSCREEN STREQUAL "ON" OR WITH_OFFSCREEN STREQUAL "ON")
    list (APPEND PLUGINS offscreen)
endif ()

include("${MINKO_HOME}/framework/enable.cmake")
foreach(PLUGIN ${PLUGINS})
    include("${MINKO_HOME}/plugin/${PLUGIN}/enable.cmake")
endforeach()