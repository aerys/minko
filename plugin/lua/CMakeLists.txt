cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-lua")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/LuaGlue/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/include/*.h"
)

list (APPEND
    ${PROJECT_NAME}_EXCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/src/luac.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/src/lua.c"
)

foreach (OBJ ${PROJECT_NAME}_EXCLUDES)
    list(REMOVE_ITEM ${PROJECT_NAME}_SRC OBJ)
endforeach ()

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/LuaGlue/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/include"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

target_compile_options (${PROJECT_NAME} PRIVATE "-DLUA_COMPAT_ALL")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options (${PROJECT_NAME} PRIVATE "-DLUA_USE_APICHECK")
endif ()

if (EMSCRIPTEN)
    target_compile_options (${PROJECT_NAME} PRIVATE "-DLUA_USE_POSIX")
endif ()

if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    target_compile_options (${PROJECT_NAME} PRIVATE "-DLUA_USE_POSIX")
endif ()

if (WIN32)
    target_compile_options (${PROJECT_NAME} PRIVATE "-DLUA_USE_WIN")
endif ()

if (VSCODE) # don't know the exact value
    target_compile_options (${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS")
endif ()

if (APPLE AND NOT IOS)
    target_compile_options (${PROJECT_NAME} PRIVATE "-DLUA_USE_MACOSX")
endif ()