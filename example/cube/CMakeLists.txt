cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-cube")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*"
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset"
)

minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_png (${PROJECT_NAME})

if (EMSCRIPTEN)
    minko_package_assets(
        ${PROJECT_NAME}
        EMBED
        "${CMAKE_CURRENT_SOURCE_DIR}/asset/texture/*.png"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/asset/*.glsl"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/asset/*.effect"
    )
else ()
    minko_package_assets(${PROJECT_NAME} COPY "${CMAKE_CURRENT_BINARY_DIR}/bin/asset/*.png")
endif ()
