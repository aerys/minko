cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-png")

config_output_path ()

file(GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file(GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lodepng/src"
)

if(WIN32)
    add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

add_library (${PROJECT_NAME}
    STATIC
    ${${PROJECT_NAME}_SRC}
)

target_include_directories (${PROJECT_NAME}
    PUBLIC
    ${${PROJECT_NAME}_INCLUDE}
)

project_library(${PROJECT_NAME})