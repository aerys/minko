cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-offscreen")

if (ANDROID OR IOS OR EMSCRIPTEN)
    return ()
endif ()

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

if (UNIX OR APPLE AND NOT IOS)
    file (GLOB
        ${PROJECT_NAME}_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${MINKO_HOME}/plugin/sdl/include"
    )
endif ()

if (WIN32)
    file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/osmesa DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH}/lib)
    file (GLOB
        ${PROJECT_NAME}_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${MINKO_HOME}/plugin/sdl/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/osmesa/windows/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/opengl/include/GL"
    )
endif ()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

