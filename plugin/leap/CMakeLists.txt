cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-leap")

if (IOS OR ANDROID OR EMSCRIPTEN)
    return ()
endif ()

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/leap/include"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})