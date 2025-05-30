cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/asset DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

file (
    GLOB_RECURSE
    JAVA_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.java"
)

foreach(JAVA_FILE ${JAVA_FILES})
    file (RELATIVE_PATH RELATIVE_JAVA_FILE_PATH ${MINKO_HOME}/../ ${JAVA_FILE})
    configure_file (${JAVA_FILE} ${MINKO_HOME}/${RELATIVE_JAVA_FILE_PATH} COPYONLY)
endforeach()

if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    return ()
endif ()

set (PROJECT_NAME "minko-plugin-video-camera")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/video/ios"
)

if (NOT ANDROID)
    file (GLOB
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/video/AndroidVideoCamera.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/video/AndroidVideoCamera.cpp"
    )
    foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
        list(REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
    endforeach ()
endif ()

if (NOT EMSCRIPTEN)
    file (GLOB
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/video/HTML5VideoCamera.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/video/HTML5VideoCamera.cpp"
    )
    foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
        list(REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
    endforeach ()
endif ()

if (NOT IOS)
    file (GLOB_RECURSE
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/video/ios/*"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/video/ios/*"
    )
    foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
        list (REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
    endforeach ()
endif ()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_sdl (${PROJECT_NAME})

if (IOS)
    target_compile_options (${PROJECT_NAME} PRIVATE -x objective-c++)
endif ()