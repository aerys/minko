cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

if (WIN32)
    set (PROJECT_NAME "minko-plugin-angle")

    file (
        GLOB_RECURSE
        ${PROJECT_NAME}_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    )

    file (
        GLOB
        ${PROJECT_NAME}_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${MINKO_HOME}/plugin/sdl/include"
        "${MINKO_HOME}/plugin/sdl/lib/sdl/include"
    )

    minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
    target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})
    
    if (WIN32 AND DLL STREQUAL "ON")
        find_library (SDL2_LIB SDL2 HINTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/windows${BITNESS}")
        find_library (SDL2_MAIN_LIB SDL2main HINTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/windows${BITNESS}")
        find_library (SDL2_MIXER_LIB SDL2_mixer HINTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/windows${BITNESS}")
        target_link_libraries (
            ${PROJECT_NAME}
            ${SDL2_LIB}
            ${SDL2_MAIN_LIB}
            ${SDL2_MIXER_LIB}
            "minko-plugin-sdl"
        )
        target_include_directories (${PROJECT_NAME} PRIVATE "${MINKO_HOME}/plugin/sdl")
    endif ()
endif ()
