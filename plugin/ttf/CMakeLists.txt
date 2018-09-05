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

minko_add_library (${PROJECT_NAME} "STATIC" "${${PROJECT_NAME}_SRC}")

target_include_directories(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

if (WIN32 OR LINUX)
    target_compile_options(${PROJECT_NAME} PRIVATE "-DMINKO_PLUGIN_TTF_FREETYPE")
endif ()

if (WIN32 AND DLL STREQUAL "ON")
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype/lib/windows${BITNESS}/freetype28.lib")
endif ()