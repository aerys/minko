cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-vr")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/opengl/include"
)

if (NOT ANDROID AND NOT IOS AND NOT EMSCRIPTEN)
    list (APPEND
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/vr/Cardboard.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/vr/Cardboard.cpp"
    )
endif ()

if (EMSCRIPTEN OR ANDROID OR IOS)
    list (APPEND
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/vr/OculusRift.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/vr/OculusRift.cpp"
    )
endif ()

if (NOT EMSCRIPTEN)
    list (APPEND
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/vr/WebVR.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/vr/WebVR.cpp"
    )
endif ()

foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
    list (REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
endforeach ()

# CREATE LIB HERE!
minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

if (ANDROID OR IOS OR EMSCRIPTEN)
    minko_enable_plugin_sensors(${PROJECT_NAME})
endif ()

if (WIN32)
    file (GLOB_RECURSE MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/*.cpp")
    file (GLOB
        WIN32_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/include"
    )
    file (GLOB_RECURSE
        MAIN_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/src/CAPI/D3D1X/CAPI_D3D1X_Util.*"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/src/CAPI/D3D1X/CAPI_D3D1X_DistortionRenderer.*"
    )
    target_compile_options (${PROJECT_NAME} PRIVATE
        "-DUNICODE"
        "-D_UNICODE"
        "-DWIN32"
    )
    target_include_directories (${PROJECT_NAME} PRIVATE ${WIN32_INCLUDE})
endif ()

if (LINUX)
    file (GLOB_RECURSE MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/linux/*.cpp")
    file (GLOB
        LINUX_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/linux/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/linux/include"
    )
    target_include_directories (${PROJECT_NAME} PRIVATE ${LINUX_INCLUDE})
endif ()

if (APPLE AND NOT IOS)
    target_compile_options (${PROJECT_NAME} PRIVATE -x objective-c++)
    file (GLOB_RECURSE
        MAIN_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/osx/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/osx/src/Util/Util_SystemInfo_OSX.mm" 
    )
    file (GLOB
        OSX_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/osx/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/osx/include"
    )
    target_include_directories (${PROJECT_NAME} PRIVATE ${OSX_INCLUDE})
endif ()

if (NOT EMSCRIPTEN)
    file (GLOB_RECURSE
        MAIN_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/src/CAPI/D3D1X/CAPI_D3D1X_Util.*"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libovr/windows/src/CAPI/D3D1X/CAPI_D3D1X_DistortionRenderer.*"
    )
endif ()

if (NOT ANDROID AND NOT IOS)
    foreach (OBJ ${MAIN_EXCLUDES})
        list (REMOVE_ITEM MAIN_SRC ${OBJ})
    endforeach ()
endif ()

target_sources (${PROJECT_NAME} PRIVATE ${MAIN_SRC})