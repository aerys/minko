include ("${MINKO_HOME}/plugins.cmake")
include ("${MINKO_HOME}/package.cmake")
include ("${MINKO_HOME}/minko.cmake")
include ("${MINKO_HOME}/project.cmake")
include ("${MINKO_HOME}/copy.cmake")

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

foreach(PLUGIN ${PLUGINS})
    include("${MINKO_HOME}/plugin/${PLUGIN}/enable.cmake")
endforeach()
