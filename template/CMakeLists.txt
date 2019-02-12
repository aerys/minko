cmake_minimum_required(VERSION 3.5.1)

if (NOT MINKO_HOME)
    if ("$ENV{MINKO_HOME}" STREQUAL "")
        message (
            FATAL_ERROR 
            "You must define the environment variable MINKO_HOME."
        )
    else ()
        set (MINKO_HOME $ENV{MINKO_HOME})
    endif ()
endif ()

if (NOT EXISTS ${MINKO_HOME}/cmake/sdk.cmake)
    message (
        FATAL_ERROR 
        "MINKO_HOME does not point to a valid Minko SDK."
    )
endif ()

include("${MINKO_HOME}/cmake/sdk.cmake")

set (PROJECT_NAME "my-project")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if (IOS)
    list (APPEND ${PROJECT_NAME}_SRC "Images.xcassets")
    set (CMAKE_IPHONEOS_DEPLOYMENT_TARGET "7.0")
endif ()

if (NOT ANDROID)
    minko_add_executable (${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")
else ()
    minko_add_library (${PROJECT_NAME} "SHARED" "${${PROJECT_NAME}_SRC}")
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_sdl (${PROJECT_NAME})

if (EMSCRIPTEN)
    minko_package_assets ("*.png;*.glsl;*.effect" "embed")
else ()
    minko_package_assets ("*.png;*.glsl;*.effect" "copy")
endif ()
