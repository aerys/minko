cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-ssl")

if (EMSCRIPTEN)
    return ()
endif ()

minko_set_bitness()
if (ANDROID)
    # Built with the NDK of the engine (see cmake/minko.cmake).
    if (NOT MINKO_ANDROID_NDK_MAJOR)
        message (FATAL_ERROR "MINKO_ANDROID_NDK_MAJOR is not set: include cmake/minko.cmake (or cmake/sdk.cmake) once the Android toolchain is loaded.")
    endif ()
    set (PLATFORM_NAME "android/${ANDROID_ABI}/r${MINKO_ANDROID_NDK_MAJOR}")
elseif (LINUX AND BITNESS EQUAL 64)
    set (PLATFORM_NAME "linux/amd64/gcc-9.4")
elseif (WIN32 AND BITNESS EQUAL 64)
    set (PLATFORM_NAME "windows/amd64/msvc")
else ()
    message(FATAL_ERROR "Platform not supported.")
endif ()

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/openssl/${PLATFORM_NAME}/include"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})
