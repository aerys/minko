cmake_minimum_required (VERSION 3.5.1)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITNESS 64)
else ()
    set(BITNESS 32)
endif ()

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif ()

string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

set (OUTPUT_FRAMEWORK_PATH
    "${CMAKE_BINARY_DIR}/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_FRAMEWORK_PATH})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_FRAMEWORK_PATH})

set (CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -MP -std=c++11 -DDEBUG -DJSON_IS_AMALGAMATION"
)
set (CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -MP -std=c++11 -DDEBUG -DJSON_IS_AMALGAMATION"
)
set (CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -MP -std=c++11 -DNDEBUG -DJSON_IS_AMALGAMATION"
)
set (CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} -MP -std=c++11 -DNDEBUG -DJSON_IS_AMALGAMATION"
)

if (UNIX)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -MMD -m${BITNESS}")
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MMD -m${BITNESS}")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -MMD -m${BITNESS}")
    set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MMD -m${BITNESS}")
elseif (WIN32)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_USE_MATH_DEFINES")
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_USE_MATH_DEFINES")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_USE_MATH_DEFINES")
    set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_USE_MATH_DEFINES")
    # add a flag to remove the error C2338 on Visual Studio 15 2017 (https://stackoverflow.com/a/30811901/10069938)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
endif ()

option (WITH_GLSL_OPTIMIZER
    "handle with-glsl-optimizer option"
    OFF
)

file (GLOB_RECURSE FRAMEWORK_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/*"
)

file (GLOB_RECURSE JSON_CPP_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp/src/*.c"
)

file (GLOB_RECURSE GLM_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm/*.inl"
)

file (GLOB_RECURSE SPARSEHASH_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/sparsehash/src/sparsehash/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/sparsehash/src/sparsehash/*.cc"
    if (WIN32)
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/sparsehash/include/windows/*.h"
    else ()
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/sparsehash/include/*.h"
    endif ()
)


if (WIN32)
    add_definitions("/wd4996")
    file (GLOB_RECURSE GLEW_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/*.h"
    )
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/lib/windows${BITNESS}")
    if (BITNESS EQUAL 32)
        add_definitions("/wd4503")
    endif ()
endif ()

if (ANDROID)
    file (GLOB_RECURSE ANDROID_SRC
        "lib/android/*.cpp"
        "lib/android/*.h"
    )
    add_definitions(-DANDROID)
endif ()


add_library (minko-framework
    STATIC
    ${JSON_CPP_SRC}
    ${GLM_SRC}
    ${FRAMEWORK_SRC}
    $<$<CONFIG:Release>:${SPARSEHASH_SRC}>
    ${GLEW_SRC}
    ${ANDROID_SRC}
)


target_include_directories (minko-framework
    PUBLIC
    "include" "src" "lib/jsoncpp/src" "lib/glm" "lib/sparsehash/src"
    if (WIN32)
        "lib/sparsehash/include/windows"
        "lib/glew/include"
    else ()
        "lib/sparsehash/include"
    endif ()
    if (ANDROID)
        "lib/android"
    endif ()
)

if (WITH_GLSL_OPTIMIZER)
    file (GLOB_RECURSE GLSL_OPTIMIZER_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/getopt/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/getopt/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/glsl/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/glsl/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/glsl/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa/main/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa/main/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa/program/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa/program/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/util/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/util/*.c"
    )

    file (GLOB GLSL_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/getopt"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/glsl/glcpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa/main"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/mesa/program"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glsl-optimizer/src/util"
    )

    list (REMOVE_ITEM
        GLSL_OPTIMIZER_SRC
        "lib/glsl-optimizer/src/glsl/main.cpp"
    )
    
    target_sources (minko-framework
        PUBLIC
        ${GLSL_OPTIMIZER_SRC}
    )

    target_include_directories (minko-framework
        PUBLIC
        ${GLSL_INCLUDE}
    )

    if (UNIX OR ANDROID)
        set_target_properties (minko-framework
        PROPERTIES
        COMPILE_FLAGS "-fno-strict-aliasing")
    endif ()

endif ()

add_custom_command (TARGET minko-framework
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_FRAMEWORK_PATH}
)
if (UNIX)
set_target_properties (minko-framework
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY
    ${OUTPUT_FRAMEWORK_PATH}
)
endif ()