cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH}/lib/freetype)

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
    "lib/freetype/include/freetype"
    "lib/freetype/include"
)

minko_add_library (${PROJECT_NAME} "STATIC" "${${PROJECT_NAME}_SRC}")

target_include_directories(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

if (WIN32 OR LINUX)
    target_compile_options(${PROJECT_NAME} PRIVATE "-DMINKO_PLUGIN_TTF_FREETYPE")
endif ()

if (WIN32 AND DLL STREQUAL "ON")
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype/lib/windows${BITNESS}/freetype.lib")
endif ()