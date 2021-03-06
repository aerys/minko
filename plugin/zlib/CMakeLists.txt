cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})


set (PROJECT_NAME "minko-plugin-zlib")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib/*.h"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib"  
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")

target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

# not too sure how to manage 'configuration { "vs*" }'
if (WIN32)
    target_compile_options (${PROJECT_NAME} PRIVATE
        -D_CRT_SECURE_NO_WARNINGS
        "/wd4996"
    )
endif ()