cmake_minimum_required (VERSION 3.5.1)

set (PROJECT_NAME "minko-test")

file (GLOB_RECURSE ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*"
)

minko_add_executable (${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_compile_options (${PROJECT_NAME} PRIVATE "-DMINKO_TEST")

minko_enable_plugin_sdl (${PROJECT_NAME})
minko_enable_plugin_serializer (${PROJECT_NAME})
minko_enable_plugin_websocket (${PROJECT_NAME})
minko_enable_plugin_http_loader (${PROJECT_NAME})
minko_enable_plugin_ssl (${PROJECT_NAME})
if (WITH_OFFSCREN STREQUAL "on" OR WITH_OFFSCREN STREQUAL "ON")
    minko_enable_plugin_offscreen (${PROJECT_NAME})
endif ()

file (GLOB GOOGLETEST_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/src/gtest-all.cc"
)

file (GLOB GOOGLETEST_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest"
)

list (REMOVE_ITEM GOOGLETEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/gtest_main.cc")

target_sources (${PROJECT_NAME} PRIVATE "${GOOGLETEST_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE "${GOOGLETEST_INCLUDE}")

if (APPLE AND NOT IOS OR LINUX)
    target_link_libraries (${PROJECT_NAME} "pthread")
endif ()

minko_package_assets (
    ${PROJECT_NAME}
    EMBED
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*/*.effect"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*/*/*.effect"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*/*/*/*.effect"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*/*/*/*/*.effect"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/effect/*.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/scene/*.scene"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/scene/*.texture"
)

target_include_directories (${PROJECT_NAME} PRIVATE "${EMSCRIPTEN}/system/lib/libcxxabi/include")