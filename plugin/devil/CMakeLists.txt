cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-devil")

if (ANDROID OR IOS)
    return()
endif ()

# Generates DevIl's config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-IL/include/config.h.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-IL/include/config.h)

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

file (
    GLOB
    ${PROJECT_NAME}_EXCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-ILU/ilur/ilur.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-IL/src/il_main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/wrjpgcom.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/jpegtran.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/djpeg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/cjpeg.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/ckconfig.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/example.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg/rdjpgcom.c"
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-IL/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-ILU/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/src/src-ILUT/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjpeg"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libtiff"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libpng"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/libjasper/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/devil/lib/lcms/include"
)

foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
    list(REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
endforeach ()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_zlib (${PROJECT_NAME})

target_compile_options (${PROJECT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS -DIL_STATIC_LIB)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "Clang" OR APPLE OR EMSCRIPTEN)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -DNOINLINE
        -Wno-implicit-function-declaration
        -Wno-format
        -Wno-int-to-void-pointer-cast
        -Wno-incompatible-pointer-types
        -Wno-unused-value
        -Wno-sizeof-pointer-memaccess
        -Wno-tautological-constant-out-of-range-compare
        -Wno-deprecated-declarations
    )
endif()