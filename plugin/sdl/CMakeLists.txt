cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-sdl")

config_output_path ()

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

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
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
    add_definitions("-x objective-c++")
endif ()

if (NOT ANDROID)
    list (REMOVE_ITEM 
        ${PROJECT_NAME}_SRC 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/file/APKProtocol.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/file/APKProtocol.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/file/APKProtocolWorker.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/file/APKProtocolWorker.cpp"
    )

add_library (${PROJECT_NAME}
    STATIC
    ${${PROJECT_NAME}_SRC}
)

target_include_directories (${PROJECT_NAME}
    PUBLIC
    ${${PROJECT_NAME}_INCLUDE}
)

project_application(${PROJECT_NAME})

#add requested offscreen

endif ()