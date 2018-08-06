cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-serializer")

file (GLOB_RECURSE
²   ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.cpp"
)

file (GLOB ${PROJECT_NAME}_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src")

list (APPEND
    ${PROJECT_NAME}_PLUGINS
    sdl
    serializer
    jpeg
    assimp
    png
)

if (NOT ANDROID)
    minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
else ()
    minko_add_library(${PROJECT_NAME} "SHARED" "${${PROJECT_NAME}_SRC}")
endif ()

foreach (${PROJECT_NAME}_PLUGIN ${${PROJECT_NAME}_PLUGINS})
    call_plugin (enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
endforeach ()

target_link_libraries (${PROJECT_NAME} "minko-framework")

if (ANDROID)
    include(${MINKO_HOME}/android-build.cmake)
    build_android(${PROJECT_NAME} "libminko-example-serializer.so")
endif ()

