cmake_minimum_required(VERSION 3.5.1)

if (NOT ANDROID)
    return ()
endif ()

set (PROJECT_NAME "minko-plugin-nodejs-worker")

if (WIN32)
    execute_process(
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/script/build_nodejs.sh --use-prebuilt
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else ()
    execute_process(
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/script/build_nodejs.sh --use-prebuilt
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif ()

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/node/deps/uv/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/node/deps/v8/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/node/src"
)

minko_add_worker (${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})
minko_enable_plugin_sdl (${PROJECT_NAME})