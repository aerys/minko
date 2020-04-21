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

file (
    GLOB_RECURSE
    ASSIMP_LIB_SRC
    "lib/assimp/code/*.c"
    "lib/assimp/code/*.h"
    "lib/assimp/code/*.cpp"
    "lib/assimp/code/*.hpp"
    "lib/assimp/contrib/*.c"
    "lib/assimp/contrib/*.cc"
    "lib/assimp/contrib/*.h"
    "lib/assimp/contrib/*.cpp"
    "lib/assimp/contrib/*.hpp"
)

list (
    REMOVE_ITEM
    ASSIMP_LIB_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/Assimp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/STLExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/PlyExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/ColladaExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/ObjExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/XFileExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/AssbinExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/3DSExporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/Exporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/AssimpCExport.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/OpenDDLParser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/DDLNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/contrib/openddlparser/code/Value.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/C4DImporter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp/code/OpenGEXImporter.cpp"
)

file (
    GLOB
    ASSIMP_LIB_INCLUDE
    "lib/assimp"
    "lib/assimp/code"
    "lib/assimp/code/BoostWorkaround"
    "lib/assimp/include"
    "lib/assimp/contrib/clipper"
    "lib/assimp/contrib/ConvertUTF"
    "lib/assimp/contrib/irrXML"
    "lib/assimp/contrib/poly2tri"
    "lib/assimp/contrib/unzip"    
)

# minko_add_library ("assimp" STATIC "${ASSIMP_LIB_SRC}")
add_library("assimp" STATIC "${ASSIMP_LIB_SRC}")
target_include_directories ("assimp" PUBLIC ${ASSIMP_LIB_INCLUDE})
target_link_libraries ("assimp" "minko-plugin-zlib")
target_include_directories ("assimp" PUBLIC "${MINKO_HOME}/plugin/zlib/lib/zlib")
target_compile_options ("assimp" PUBLIC -DMINKO_PLUGIN_ZLIB)
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
target_compile_options (
    "assimp"
    PRIVATE 
    -DASSIMP_BUILD_NO_OWN_ZLIB
    -DASSIMP_BUILD_NO_EXPORT
    -DASSIMP_BUILD_BOOST_WORKAROUND
    -DASSIMP_BUILD_NO_IMPORTER_INSTANCIATION
)
target_compile_options (
    ${PROJECT_NAME}
    PRIVATE 
    -DASSIMP_BUILD_NO_OWN_ZLIB
    -DASSIMP_BUILD_NO_EXPORT
    -DASSIMP_BUILD_BOOST_WORKAROUND
    -DASSIMP_BUILD_NO_IMPORTER_INSTANCIATION
)

if (MSVC)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE 
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -DAI_LMW_MAX_WEIGHTS=4
        # OpenGL ES 2 max vertex count
        -DAI_SLM_DEFAULT_MAX_VERTICES=128000
        # OpenGL ES 2 max indices/triangles count
        -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
    )
    target_compile_options (
        assimp PRIVATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -DAI_LMW_MAX_WEIGHTS=4
        # OpenGL ES 2 max vertex count
        -DAI_SLM_DEFAULT_MAX_VERTICES=128000
        # OpenGL ES 2 max indices/triangles count
        -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
    )
else ()
    target_compile_options (
        ${PROJECT_NAME} PRIVATE 
        -DAI_LMW_MAX_WEIGHTS=4
        # OpenGL ES 2 max vertex count
        -DAI_SLM_DEFAULT_MAX_VERTICES=128000
        # OpenGL ES 2 max indices/triangles count
        -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
    )
    target_compile_options (
        assimp PRIVATE 
        -DAI_LMW_MAX_WEIGHTS=4
        # OpenGL ES 2 max vertex count
        -DAI_SLM_DEFAULT_MAX_VERTICES=128000
        # OpenGL ES 2 max indices/triangles count
        -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
    )
endif ()

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

### Silents some warnings when building assimp library.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
    CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR
    EMSCRIPTEN)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wno-deprecated-declarations)
    target_compile_options(assimp PRIVATE
        -Wno-deprecated-declarations)
endif ()