cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-serializer")

add_subdirectory("lib/msgpack-c")
TARGET_COMPILE_DEFINITIONS(msgpack-cxx INTERFACE MSGPACK_NO_BOOST)

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${msgpack-cxx_HEADERS}"
)


file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/msgpack-c/include"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_jpeg (${PROJECT_NAME})
minko_enable_plugin_png (${PROJECT_NAME})
minko_enable_plugin_ttf (${PROJECT_NAME})

if (WIN32)
    target_compile_options (${PROJECT_NAME} PRIVATE
        -D_LIB
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D__STDC_VERSION__=199901L
        -D__STDC__
        -DWIN32
        -D_WINDLL_IMPORT
        "/wd4028"
        "/wd4244"
        "/wd4267"
        "/wd4996"
        "/wd4273"
        "/wd4503"
    )
endif ()

if (LINUX)
    target_compile_options (${PROJECT_NAME} PRIVATE
        -D__STDC_FORMAT_MACROS
        "-Wno-deprecated-declarations"
        "-Wno-deprecated"
    )
endif ()

if (WITH_TEXTURE_COMPRESSOR STREQUAL "ON")
    file (GLOB_RECURSE
        TEXTURE_COMP_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.cpp"
    )

    file (GLOB
        TEXTURE_COMP_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/inc"
    )

    target_include_directories (${PROJECT_NAME} PRIVATE ${TEXTURE_COMP_INCLUDE})

    list (REMOVE_ITEM
        TEXTURE_COMP_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_win32_threading.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_win32_threading.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_timer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_timer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_jpgd.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_jpge.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_winhdr.h"
    )

    target_compile_options (${PROJECT_NAME} PRIVATE
        -DMINIZ_NO_TIME
        -DCRNLIB_NO_ETC1
    )

    if (LINUX)
        list (REMOVE_ITEM
            TEXTURE_COMP_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_threading_win32.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_threading_win32.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_LzFindMt.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_LzFindMt.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_Threads.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_Threads.cpp"
        )
    endif ()

    if (LINUX AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_compile_options (${PROJECT_NAME} PRIVATE
            -D_DEBUG
        )
    endif ()

    if (ANDROID OR IOS OR EMSCRIPTEN)
        target_compile_options (${PROJECT_NAME} PRIVATE
            -DMINKO_NO_PVRTEXTOOL
            -DMINKO_NO_QCOMPRESS
            -DMINKO_NO_CRNLIB
        )
    endif ()

    if (APPLE AND IOS)
        target_compile_options (${PROJECT_NAME} PRIVATE
            -DMINKO_NO_QCOMPRESS
            -DMINKO_NO_CRNLIB
        )
    endif ()

    if (BITNESS EQUAL 64)
        target_compile_options (${PROJECT_NAME} PRIVATE
            -DMINKO_NO_QCOMPRESS
        )
    endif ()

    if (WIN32 OR UNIX OR APPLE)
        file (GLOB
            OPT_INCLUDE_1
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/PVRTexTool/Include"
        )
        target_include_directories (${PROJECT_NAME} PRIVATE ${OPT_INCLUDE_1})
    endif ()

    if ((WIN32 OR UNIX) AND BITNESS EQUAL 32)
        file (GLOB
            OPT_INCLUDE_2
            "lib/QCompress/Inc"
        )
        target_include_directories (${PROJECT_NAME} PRIVATE ${OPT_INCLUDE_2})
    endif()
else ()
    target_compile_options (${PROJECT_NAME} PRIVATE
        -DMINKO_NO_PVRTEXTOOL
        -DMINKO_NO_QCOMPRESS
        -DMINKO_NO_CRNLIB
    )
endif ()

target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

target_sources (${PROJECT_NAME}
    PRIVATE
    ${TEXTURE_COMP_SRC}
)
