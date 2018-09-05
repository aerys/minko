cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-jpeg")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "lib/*.hpp"
    "lib/*.h"
    "lib/*.cpp"
    "lib/*.c"
    "include.*.hpp"
    "src/*.cpp"    
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "include"
    "lib/jpeg-compressor/src"
)

minko_add_library (${PROJECT_NAME} "STATIC" "${${PROJECT_NAME}_SRC}")
target_include_directories(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})