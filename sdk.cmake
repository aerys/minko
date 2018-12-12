include ("${MINKO_HOME}/plugins.cmake")
include ("${MINKO_HOME}/package.cmake")
include ("${MINKO_HOME}/minko.cmake")
include ("${MINKO_HOME}/project.cmake")
include ("${MINKO_HOME}/copy.cmake")
include ("${MINKO_HOME}/android-build.cmake")
include ("${MINKO_HOME}/cpjf.cmake")


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

if (WITH_NODEJS_WORKER STREQUAL "on" OR WITH_NODEJS_WORKER STREQUAL "ON")
    list (APPEND PLUGINS nodejs-worker)
endif ()

if (WITH_OFFSCREEN STREQUAL "on" OR WITH_OFFSCREEN STREQUAL "ON")
    list (APPEND PLUGINS offscreen)
endif ()

foreach(PLUGIN ${PLUGINS})
    include("${MINKO_HOME}/plugin/${PLUGIN}/enable.cmake")
endforeach()