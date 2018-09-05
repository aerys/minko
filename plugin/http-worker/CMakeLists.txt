cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-http-worker")

if (EMSCRIPTEN)
    return ()
endif ()

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/include"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})
target_compile_options (${PROJECT_NAME} PRIVATE -DCURL_STATICLIB)

if (UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries (${PROJECT_NAME} "curl")
endif ()

if (DLL)
    target_link_libraries (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/windows${BITNESS}/${BUILD_TYPE}/libcurl.lib")
endif ()

if (APPLE AND NOT IOS)
    target_link_libraries (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/lib/osx64/release/libcurl.a" "-framework Security")
    minko_plugin_link ("zlib" ${PROJECT_NAME})
endif ()

if (ANDROID)
    target_link_libraries (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/lib/android/release/libcurl.a")
    minko_plugin_link ("zlib" ${PROJECT_NAME})
endif ()