cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-assimp")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "src/*.hpp"
    "src/*.h"
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "include"
    "src"
)

list (APPEND
    ${PROJECT_NAME}_PLUGINS
    zlib
)

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE})

foreach (${PROJECT_NAME}_PLUGIN ${${PROJECT_NAME}_PLUGINS})
    call_plugin (enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
endforeach ()

file (GLOB_RECURSE
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

list (REMOVE_ITEM
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

file (GLOB
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
minko_add_library ("libassimp" STATIC "${ASSIMP_LIB_SRC}")
target_include_directories("libassimp" PUBLIC ${ASSIMP_LIB_INCLUDE})

target_link_libraries("libassimp" "minko-plugin-zlib")
target_include_directories("libassimp" PUBLIC "${MINKO_HOME}/plugin/zlib/lib/zlib")
target_compile_options ("libassimp" PUBLIC
    -DMINKO_PLUGIN_ZLIB
)
target_link_libraries(${PROJECT_NAME} "libassimp")

target_compile_options("libassimp" PUBLIC 
    -DASSIMP_BUILD_NO_OWN_ZLIB
    -DASSIMP_BUILD_NO_EXPORT
    -DASSIMP_BUILD_BOOST_WORKAROUND
    -DASSIMP_BUILD_NO_IMPORTER_INSTANCIATION
)

target_compile_options(${PROJECT_NAME} PUBLIC 
    -DASSIMP_BUILD_NO_OWN_ZLIB
    -DASSIMP_BUILD_NO_EXPORT
    -DASSIMP_BUILD_BOOST_WORKAROUND
    -DASSIMP_BUILD_NO_IMPORTER_INSTANCIATION
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC 
        -D_CRT_SECURE_NO_WARNINGS
	    -D_SCL_SECURE_NO_WARNINGS
	    -DAI_LMW_MAX_WEIGHTS=4
	    # OpenGL ES 2 max vertex count
	    -DAI_SLM_DEFAULT_MAX_VERTICES=128000
	    # OpenGL ES 2 max indices/triangles count
	    -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
    )
else ()
    target_compile_options(${PROJECT_NAME} PUBLIC 
        -DAI_LMW_MAX_WEIGHTS=4
        # OpenGL ES 2 max vertex count
        -DAI_SLM_DEFAULT_MAX_VERTICES=128000
        # OpenGL ES 2 max indices/triangles count
        -DAI_SLM_DEFAULT_MAX_TRIANGLES=21845
    )
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR APPLE)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wno-unknown-warning-option
        -Wno-deprecated-register
    )
endif ()

if (APPLE AND NOT IOS)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wno-attributes
        -Wno-deprecated-register
    )
endif ()

if (EMSCRIPTEN)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wno-attributes
        -Wimplicit-function-declaration
        -Wparentheses-equality
    )
endif ()

if (ANDROID)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wno-attributes
    )
endif ()