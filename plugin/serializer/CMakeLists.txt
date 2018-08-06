cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-serializer")

list (APPEND
    ${PROJECT_NAME}_PLUGINS
    jpeg
    png
    ttf
)

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/msgpack-c/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/msgpack-c/include/*.hpp"
)

# message ("OMG: ${PROJECT_NAME}")

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/msgpack-c/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/msgpack-c/src"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE})

foreach (${PROJECT_NAME}_PLUGIN ${${PROJECT_NAME}_PLUGINS})
    call_plugin (enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
endforeach ()

if (WIN32)
    target_compile_options (${PROJECT_NAME} PUBLIC
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

if (UNIX AND NOT IOS AND NOT EMSCRIPTEN)
    target_compile_options (${PROJECT_NAME} PUBLIC
        -D__STDC_FORMAT_MACROS
        "-Wno-deprecated-declarations"
    )
endif ()

if (WITH_TEXTURE_COMPRESSOR STREQUAL "on")
    file (GLOB_RECURSE
        TEXTURE_COMP_SRC
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.h"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.hpp"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.c"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/*.cpp"
    )

    file (GLOB
        TEXTURE_COMP_INCLUDE
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib"
		"{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/inc"
    )
    target_include_directories (${PROJECT_NAME} PUBLIC ${TEXTURE_COMP_INCLUDE})

    list (REMOVE_ITEM
        TEXTURE_COMP_SRC
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_win32_threading.h"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_win32_threading.cpp"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_timer.h"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzham_timer.cpp"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_jpgd.cpp"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_jpge.cpp"
        "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_winhdr.h"
    )

    target_compile_options (${PROJECT_NAME} PUBLIC
        -DMINIZ_NO_TIME
        -DCRNLIB_NO_ETC1
    )

    if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        list (REMOVE_ITEM
            TEXTURE_COMP_SRC
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_threading_win32.h"
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/crn_threading_win32.cpp"
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_LzFindMt.h"
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_LzFindMt.cpp"
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_Threads.h"
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/crnlib/crnlib/lzma_Threads.cpp"
        )
    endif ()

    if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_compile_options (${PROJECT_NAME} PUBLIC
            -D_DEBUG
        )
    endif ()

    if (ANDROID OR IOS OR EMSCRIPTEN)
        target_compile_options (${PROJECT_NAME} PUBLIC
            -DMINKO_NO_PVRTEXTOOL
            -DMINKO_NO_QCOMPRESS
            -DMINKO_NO_CRNLIB
        )
    endif ()
    
    if (APPLE AND IOS)
        target_compile_options (${PROJECT_NAME} PUBLIC
            -DMINKO_NO_QCOMPRESS
            -DMINKO_NO_CRNLIB
        )
    endif ()

    if (BITNESS EQUAL 32)
        target_compile_options (${PROJECT_NAME} PUBLIC
            "MINKO_NO_QCOMPRESS"
        )
    endif ()

    if (WIN32 AND UNIX AND APPLE AND NOT IOS AND NOT EMSCRIPTEN)
        file (GLOB
            OPT_INCLUDE_1
            "{CMAKE_CURRENT_SOURCE_DIR}/lib/PVRTexTool/Include"
        )
        target_include_directories (${PROJECT_NAME} PUBLIC ${OPT_INCLUDE_1})
    endif ()

    if (BITNESS EQUAL 32)
        file (GLOB
            OPT_INCLUDE_2
            "lib/QCompress/Inc"
        )
        target_include_directories (${PROJECT_NAME} PUBLIC ${OPT_INCLUDE_2})
    endif()
else ()
    target_compile_options (${PROJECT_NAME} PUBLIC
        -DMINKO_NO_PVRTEXTOOL
        -DMINKO_NO_QCOMPRESS
        -DMINKO_NO_CRNLIB
    )
endif ()

target_include_directories (${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE})

target_sources (${PROJECT_NAME}
    PUBLIC
    ${TEXTURE_COMP_SRC}
)