cmake_minimum_required(VERSION 3.5.1)

if (IOS OR ANDROID)
    return ()
endif ()

set (PROJECT_NAME "minko-example-pbr")

file (GLOB ${PROJECT_NAME}_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*"
)

minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")

target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_jpeg (${PROJECT_NAME})
minko_enable_plugin_fx (${PROJECT_NAME})

minko_package_assets (
    ${PROJECT_NAME}
    EMBED
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*.jpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*.effect"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/texture/*.jpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/texture/*/*.jpg"
)
