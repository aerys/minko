cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

file (
    GLOB_RECURSE
    JAVA_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.java"
)

foreach(JAVA_FILE ${JAVA_FILES})
    file (RELATIVE_PATH RELATIVE_JAVA_FILE_PATH ${MINKO_HOME}/../ ${JAVA_FILE})
    configure_file (${JAVA_FILE} ${MINKO_HOME}/${RELATIVE_JAVA_FILE_PATH} COPYONLY)
endforeach()

if (NOT ANDROID)
    return ()
endif ()

set (PROJECT_NAME "minko-plugin-nodejs-worker")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/node/android/${ANDROID_ABI}/r22b/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/node/android/${ANDROID_ABI}/r22b/include/node"
)

minko_add_worker (${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})
minko_enable_plugin_sdl (${PROJECT_NAME})
