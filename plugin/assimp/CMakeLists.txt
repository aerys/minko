cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-assimp")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "src/*.hpp"
    "src/*.h"
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "include"
    "src"
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_zlib (${PROJECT_NAME})

# --- Vendored Assimp v6.0.5 sources ------------------------------------------
# All importer/common/post-processing sources (exporters are filtered out below
# and additionally compiled out by ASSIMP_BUILD_NO_EXPORT).
file (
    GLOB_RECURSE
    ASSIMP_LIB_SRC
    "lib/assimp/code/*.cpp"
)

# Kept contrib sources (explicit list - do NOT glob contrib/** as that would pull
# in unrelated zlib/contrib/** sources). zlib is handled separately below.
list (
    APPEND
    ASSIMP_LIB_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/clipper/clipper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/pugixml/src/pugixml.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/unzip/unzip.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/unzip/ioapi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/OpenDDLCommon.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/DDLNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/OpenDDLParser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/Value.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/OpenDDLStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/OpenDDLExport.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/poly2tri/poly2tri/sweep/cdt.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/poly2tri/poly2tri/sweep/sweep_context.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/poly2tri/poly2tri/sweep/advancing_front.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/poly2tri/poly2tri/sweep/sweep.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/poly2tri/poly2tri/common/shapes.cc"
)

# zlib is handled separately below (we reuse the engine zlib plugin instead of
# Assimp's bundled copy - see the include-redirect comment near the bottom).

# Drop exporter sources (also guarded by ASSIMP_BUILD_NO_EXPORT) and a stale
# IFC generator that #includes a non-existent AssimpPCH.h and is not part of
# upstream's build list.
list (FILTER ASSIMP_LIB_SRC EXCLUDE REGEX ".*Exporter\\.cpp$")
list (FILTER ASSIMP_LIB_SRC EXCLUDE REGEX ".*IFCReaderGen_4\\.cpp$")

file (
    GLOB
    ASSIMP_LIB_INCLUDE
    "lib/assimp"
    "lib/assimp/include"
    "lib/assimp/code"
    "lib/assimp/contrib"
    "lib/assimp/contrib/rapidjson/include"
    "lib/assimp/contrib/pugixml/src"
    "lib/assimp/contrib/utf8cpp/source"
    "lib/assimp/contrib/unzip"
    "lib/assimp/contrib/poly2tri"
    "lib/assimp/contrib/clipper"
    "lib/assimp/contrib/openddlparser/include"
    "lib/assimp/contrib/stb"
    "lib/assimp/contrib/earcut-hpp/include"
)

add_library("assimp" STATIC "${ASSIMP_LIB_SRC}")
target_include_directories ("assimp" PUBLIC ${ASSIMP_LIB_INCLUDE})

# Resolve Assimp's <zlib.h>/"zlib.h" to the engine zlib plugin headers
# (paired with ASSIMP_BUILD_NO_OWN_ZLIB) so only one zlib is ever linked.
target_include_directories ("assimp" PRIVATE "${MINKO_HOME}/plugin/zlib/lib/zlib")
target_compile_options ("assimp" PRIVATE -DMINKO_PLUGIN_ZLIB)
# Assimp 6.x requires C++17 (the engine itself builds at C++14; this is a
# per-target override and does NOT force an engine-wide bump).
set_target_properties ("assimp" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries (${PROJECT_NAME} "assimp")

get_target_property(OUTPUT_PATH ${PROJECT_NAME} ARCHIVE_OUTPUT_DIRECTORY)
set_target_properties (
    "assimp"
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    ${OUTPUT_PATH}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    ${OUTPUT_PATH}
    ARCHIVE_OUTPUT_DIRECTORY
    ${OUTPUT_PATH}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG
    ${OUTPUT_PATH}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE
    ${OUTPUT_PATH}
    LIBRARY_OUTPUT_DIRECTORY
    ${OUTPUT_PATH}
)
# Assimp build configuration (v6.0.5):
#  - NO_EXPORT          : exporters off (import-only engine).
#  - NO_GLTF1_IMPORTER  : keep glTF2, drop legacy glTF1.
#  - NO_USD_IMPORTER    : tinyusdz contrib was pruned.
#  - NO_C4D_IMPORTER    : requires the proprietary Cinema4D (Melange) SDK.
#  - NO_M3D_IMPORTER    : M3DWrapper.cpp misses a logger include in this config
#                         (off the keep-list anyway).
#  - NO_X3D_IMPORTER    : VrmlConverter pulls a pruned meshlab/io_x3d header
#                         (X3D/VRML are off the keep-list).
# draco is opt-in (ASSIMP_BUILD_USE_DRACO) and stays off by not defining it.
# Importers come from the public importer registry. NO_OWN_ZLIB makes Assimp
# use the engine zlib.
set (ASSIMP_BUILD_DEFINES
    -DASSIMP_BUILD_NO_OWN_ZLIB
    -DASSIMP_BUILD_NO_EXPORT
    -DASSIMP_BUILD_NO_GLTF1_IMPORTER
    -DASSIMP_BUILD_NO_USD_IMPORTER
    -DASSIMP_BUILD_NO_C4D_IMPORTER
    -DASSIMP_BUILD_NO_M3D_IMPORTER
    -DASSIMP_BUILD_NO_X3D_IMPORTER
)
target_compile_options ("assimp" PRIVATE ${ASSIMP_BUILD_DEFINES})
target_compile_options (${PROJECT_NAME} PRIVATE ${ASSIMP_BUILD_DEFINES})

# OpenGL ES 2 tuning, applied to both Assimp targets: cap the bone weights per
# vertex and the SplitLargeMeshes vertex/triangle budgets to what ES2 can index.
set (ASSIMP_ES2_TUNING_DEFINES
    -DAI_LMW_MAX_WEIGHTS=4
    -DAI_SLM_DEFAULT_MAX_VERTICES=128000
    -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
)

foreach (assimpTarget ${PROJECT_NAME} assimp)
    target_compile_options (${assimpTarget} PRIVATE ${ASSIMP_ES2_TUNING_DEFINES})

    if (MSVC)
        target_compile_options (
            ${assimpTarget} PRIVATE
            -D_CRT_SECURE_NO_WARNINGS
            -D_SCL_SECURE_NO_WARNINGS
        )
    endif ()
endforeach ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR APPLE)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -Wno-unknown-warning-option
        -Wno-deprecated-register
    )
endif ()

if (APPLE AND NOT IOS)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -Wno-attributes
        -Wno-deprecated-register
    )
endif ()

if (EMSCRIPTEN)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -Wno-attributes
        -Wimplicit-function-declaration
        -Wparentheses-equality
    )
endif ()

if (ANDROID)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -Wno-attributes
    )
endif ()

### Silence deprecation warnings when building the Assimp targets.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
    CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR
    EMSCRIPTEN)
    foreach (assimpTarget ${PROJECT_NAME} assimp)
        target_compile_options (${assimpTarget} PRIVATE -Wno-deprecated-declarations)
    endforeach ()
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR "AppleClang")
    target_compile_options("assimp" PRIVATE  
        -Waddress-of-packed-member)
endif ()