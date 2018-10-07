cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-sdl")

file (GLOB_RECURSE 
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

list (REMOVE_ITEM
    ${PROJECT_NAME}_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/SDLWebGLBackend.cpp"
)

file (GLOB 
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

#add requested offscreen

if (EMSCRIPTEN)
    list (REMOVE_ITEM
        ${PROJECT_NAME}_INCLUDE 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/include"
    )
    list (APPEND 
        ${PROJECT_NAME}_INCLUDE 
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL"
    )
    list (APPEND 
        ${PROJECT_NAME}_SRC 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/SDLWebGLBackend.cpp"
    )
endif ()

if (APPLE)
    add_definitions(-x objective-c++)
endif ()

if (NOT ANDROID)
    list (REMOVE_ITEM 
        ${PROJECT_NAME}_SRC 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/file/APKProtocol.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/file/APKProtocol.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/file/APKProtocolWorker.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/file/APKProtocolWorker.cpp"
    )
endif ()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")

if (WIN32 AND DLL STREQUAL "ON")
    find_library(SDL2_LIB SDL2 HINTS "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}")
    find_library(SDL2_MAIN_LIB SDL2main HINTS "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}")
    find_library(SDL2_MIXER_LIB SDL2_mixer HINTS "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}")
    target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIB}
        ${SDL2_MAIN_LIB}
        ${SDL2_MIXER_LIB}
    )
endif ()

target_include_directories (${PROJECT_NAME}
    PRIVATE
    ${${PROJECT_NAME}_INCLUDE}
)


#add requested offscreen