cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-html-overlay")

file (GLOB ${PROJECT_NAME}_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*"
)

minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")

target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_html_overlay (${PROJECT_NAME})

minko_package_assets (
    ${PROJECT_NAME}
    EMBED
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*.effect"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*.glsl"
)
minko_package_assets (
    ${PROJECT_NAME}
    COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/html/*/*.css"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/html/*/*.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/html/*.html"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        ${PROJECT_NAME} 
        PRIVATE
        -Wno-extern-c-compat
        -Wno-objc-missing-super-calls
    )
endif ()