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

if (NOT ANDROID)
    return ()
endif ()

# Built with the NDK of the engine (see cmake/minko.cmake).
if (NOT MINKO_ANDROID_NDK_MAJOR)
    message (FATAL_ERROR "MINKO_ANDROID_NDK_MAJOR is not set: include cmake/minko.cmake (or cmake/sdk.cmake) once the Android toolchain is loaded.")
endif ()

# Only this ABI's libnode.so, with its licences and build-info.json.
set (NODE_PACKAGE_PATH "lib/node/android/${ANDROID_ABI}/r${MINKO_ANDROID_NDK_MAJOR}")
file (
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/${NODE_PACKAGE_PATH}"
    DESTINATION "${MINKO_HOME}/${RELATIVE_LIBRARY_PATH}/lib/node/android/${ANDROID_ABI}"
)

set (PROJECT_NAME "minko-plugin-nodejs-worker")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

minko_add_worker (${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
minko_enable_plugin_sdl (${PROJECT_NAME})

# NodeJSWorker.cpp declares the Node.js functions it calls instead of including
# node.h: check that libnode.so exports each of them.
add_custom_command (
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -D "NM=${CMAKE_NM}"
        -D "ARCHIVE=$<TARGET_FILE:${PROJECT_NAME}>"
        -D "LIBRARY=${CMAKE_CURRENT_SOURCE_DIR}/${NODE_PACKAGE_PATH}/lib/libnode.so"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check-node-symbols.cmake"
    VERBATIM
)
