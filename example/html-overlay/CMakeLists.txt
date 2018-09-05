cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-html-overlay")

file (GLOB ${PROJECT_NAME}_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*"
)

if (NOT ANDROID)
    minko_add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
else ()
    minko_add_library(${PROJECT_NAME} "SHARED" "${${PROJECT_NAME}_SRC}")
endif ()

target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_html_overlay (${PROJECT_NAME})

minko_copy (${CMAKE_CURRENT_SOURCE_DIR}/asset ${OUTPUT_PATH}/asset ${PROJECT_NAME})

if (EMSCRIPTEN)
    minko_package_assets ("*.effect;*.glsl" "embed")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        ${PROJECT_NAME} 
        PRIVATE
        -Wno-extern-c-compat
        -Wno-objc-missing-super-calls
    )
endif ()