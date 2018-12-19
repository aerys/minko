cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

file (
    GLOB_RECURSE
    JAVA_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.java"
)

foreach(JAVA_FILE ${JAVA_FILES})
    file (RELATIVE_PATH RELATIVE_JAVA_FILE_PATH ${MINKO_HOME}/../ ${JAVA_FILE})
    configure_file (${JAVA_FILE} ${MINKO_HOME}/${RELATIVE_JAVA_FILE_PATH} COPYONLY)
endforeach()

set (PROJECT_NAME "minko-plugin-jpeg")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "lib/*.hpp"
    "lib/*.h"
    "lib/*.cpp"
    "lib/*.c"
    "include.*.hpp"
    "src/*.cpp"    
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "include"
    "lib/jpeg-compressor/src"
)

minko_add_library (${PROJECT_NAME} "STATIC" "${${PROJECT_NAME}_SRC}")
target_include_directories(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})