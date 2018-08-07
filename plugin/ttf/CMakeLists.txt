cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-ttf")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "src/*.cpp"
    "include/*.hpp"
    "lib/freetype/*.h"
    "lib/freetype/*.c"    
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "include"
    "lib/freetype/include/freetype2"
    "lib/freetype/include"
)

minko_add_library (${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE})

target_compile_options(${PROJECT_NAME} PUBLIC "-DMINKO_PLUGIN_TTF_FREETYPE")