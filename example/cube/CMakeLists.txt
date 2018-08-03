cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-cube")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "asset/*"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "src"
    "asset"
)

list (APPEND
    ${PROJECT_NAME}_PLUGINS
    sdl
    png
)
if (NOT ANDROID)
    minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
else ()
    minko_add_library(${PROJECT_NAME} "SHARED" "${${PROJECT_NAME}_SRC}")
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    package_assets (".png;.glsl;.effect" "on")
    package_assets (".png;.glsl;.effect" "off")
else ()
    package_assets (".png;.glsl;.effect" "off")
endif ()
target_include_directories(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE} ${FRAMEWORK_INCLUDES})
foreach (${PROJECT_NAME}_PLUGIN ${${PROJECT_NAME}_PLUGINS})
    call_plugin (enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
endforeach ()
target_link_libraries (${PROJECT_NAME} "minko-framework")
if (ANDROID)
    include(${MINKO_HOME}/android-build.cmake)
    build_android(${PROJECT_NAME} "libminko-example-cube.so")
endif ()
