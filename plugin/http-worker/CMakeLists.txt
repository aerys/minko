cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-http-worker")

if (EMSCRIPTEN)
    return ()
endif ()

minko_set_bitness()
if (ANDROID)
    set (PLATFORM_NAME "android/${ANDROID_ABI}/r25b")
elseif (LINUX AND BITNESS EQUAL 64)
    set (PLATFORM_NAME "linux/amd64/gcc-9.4")
elseif (WIN32 AND BITNESS EQUAL 64)
    set (PLATFORM_NAME "windows/amd64/msvc")
else ()
    message(ERROR "Platform not supported.")
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
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/${PLATFORM_NAME}/include"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})
target_compile_options (${PROJECT_NAME} PRIVATE -DCURL_STATICLIB)

if (DLL)
    target_link_libraries (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/windows${BITNESS}/libcurl.lib")
endif ()

if (ANDROID)
    target_link_libraries (${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/${PLATFORM_NAME}/lib/libcurl.a")
    minko_plugin_link ("zlib" ${PROJECT_NAME})
endif ()
