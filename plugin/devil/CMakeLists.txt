cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-devil")

if (ANDROID OR IOS)
    return()
endif ()

### Loads 3rdParty libraries.
add_subdirectory(lib)

minko_add_library (${PROJECT_NAME} STATIC
    src/minko/file/DevILParser.cpp
    src/minko/file/DevILWriter.cpp)

### Includes directories within/for the plugin.
target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PRIVATE lib/devil/DevIL/include #[[Needed because DevIL's targets does not include their directory within.]])

### Links IL, ILU and ILUT (DevIL targets) to the plugin.
target_link_libraries(${PROJECT_NAME} IL ILU ILUT)

minko_enable_plugin_zlib (${PROJECT_NAME})

target_compile_options (${PROJECT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS -DIL_STATIC_LIB)