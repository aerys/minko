cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-sdl")

set (${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

if (NOT ANDROID)
    list (REMOVE_ITEM
        ${PROJECT_NAME}_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/file/APKProtocol.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/file/APKProtocol.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/file/APKProtocolWorker.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/file/APKProtocolWorker.cpp"
    )
endif ()

if (EMSCRIPTEN)
    list (APPEND
        ${PROJECT_NAME}_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL"
    )
    minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
    target_include_directories (${PROJECT_NAME}
        PRIVATE
        ${${PROJECT_NAME}_INCLUDE}
    )
    return ()
endif ()

list (REMOVE_ITEM
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/SDLWebGLBackend.cpp"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")

minko_set_bitness()
if (ANDROID)
    set (PLATFORM_NAME "android/armeabi-v7a/r25b")
elseif (LINUX AND BITNESS EQUAL 64)
    set (PLATFORM_NAME "linux/amd64/gcc-9.4")
elseif (WIN32 AND BITNESS EQUAL 64)
    set (PLATFORM_NAME "windows/amd64/msvc")
else ()
    message(ERROR "Platform not supported.")
endif ()

list (APPEND
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/${PLATFORM_NAME}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl_mixer/${PLATFORM_NAME}/include"
)

target_include_directories (${PROJECT_NAME}
    PRIVATE
    ${${PROJECT_NAME}_INCLUDE}
)

if (WITH_OFFSCREEN STREQUAL "ON" OR WITH_OFFSCREEN STREQUAL "ON")
    minko_enable_plugin_offscreen (${PROJECT_NAME})
    target_compile_options (${PROJECT_NAME} PRIVATE "-DSDL_AUDIO_ENABLED=0")
endif ()

