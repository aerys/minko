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

minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")

minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_serializer (${PROJECT_NAME})
minko_enable_plugin_jpeg (${PROJECT_NAME})
minko_enable_plugin_assimp (${PROJECT_NAME})
minko_enable_plugin_png (${PROJECT_NAME})

minko_package_assets (
    ${PROJECT_NAME}
    EMBED
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*.scene"
)
