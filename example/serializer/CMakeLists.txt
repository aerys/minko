cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-serializer")

file (GLOB ${PROJECT_NAME}_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.cpp"
)

if (NOT ANDROID)
    minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
else ()
    minko_add_library(${PROJECT_NAME} "SHARED" "${${PROJECT_NAME}_SRC}")
endif ()

minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_serializer (${PROJECT_NAME})
minko_enable_plugin_jpeg (${PROJECT_NAME})
minko_enable_plugin_assimp (${PROJECT_NAME})
minko_enable_plugin_png (${PROJECT_NAME})

if (EMSCRIPTEN)
    minko_package_assets ("*.scene;*.glsl;*.effect" "embed")
else ()
    minko_package_assets ("*.scene;*.glsl;*.effect" "copy")
endif ()