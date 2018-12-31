cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/WebViewJavascriptBridge DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH}/lib)
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

set (PROJECT_NAME "minko-plugin-html-overlay")

minko_add_library (${PROJECT_NAME} STATIC "")

# Include CEF toolchain
include(${MINKO_HOME}/DownloadCEF.cmake)

# Determine the platform.
set (CEF_PLATFORM "${SYSTEM_NAME}${BITNESS}")
if (APPLE AND NOT IOS)
    set (CEF_PLATFORM "macosx64")
endif ()

# Specify the CEF distribution version (in both CMakeLists.txt and enable.cmake).
set (CEF_VERSION "3.3538.1852.gcb937fc")
set (THIRD_PARTY_PATH "${MINKO_HOME}/third_party/cef/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")

# Download and extract the CEF binary distribution.
set (CEF_OUTPUT_PATH "${MINKO_HOME}/third_party/cef")
if (LINUX OR WIN32 OR APPLE AND NOT IOS)
    DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CEF_OUTPUT_PATH}")
endif ()

if (LINUX)
    add_custom_command (
        PRE_BUILD
        TARGET ${PROJECT_NAME}
        COMMAND strip ${THIRD_PARTY_PATH}/Release/libcef.so
    )
endif ()

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${THIRD_PARTY_PATH}/include/*.hpp"
    "${THIRD_PARTY_PATH}/include/*.h"
    "${THIRD_PARTY_PATH}/include/*.cpp"
    "${THIRD_PARTY_PATH}/include/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/*.hpp"
)

file (
    GLOB
    ${PROJECT_NAME}_EXCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/minko/lua/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/lua/*"
)

foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
    list (REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
endforeach ()

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_sources (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SRC})
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_http_loader (${PROJECT_NAME})
minko_enable_plugin_sdl (${PROJECT_NAME})

if (WIN32 OR LINUX)
    file (GLOB
        INCLUDE_MAIN_LIST
        "${THIRD_PARTY_PATH}"
    )

    file (
        GLOB_RECURSE
        SRC_MAIN_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/chromium/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/chromium/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/chromium/*.hpp"
        "${THIRD_PARTY_PATH}/libcef_dll/*.hpp"
        "${THIRD_PARTY_PATH}/libcef_dll/*.hh"
        "${THIRD_PARTY_PATH}/libcef_dll/*.h"
        "${THIRD_PARTY_PATH}/libcef_dll/*.cpp"
        "${THIRD_PARTY_PATH}/libcef_dll/*.cc"
        "${THIRD_PARTY_PATH}/libcef_dll/*.c"
    )

    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        "-DCHROMIUM"
        "-DV8_DEPRECATION_WARNINGS"
        "-DCHROMIUM_BUILD"
        
        "-DWRAPPING_CEF_SHARED"
        "-DBUILDING_CEF_SHARED"
        
        "-DUSE_DEFAULT_RENDER_THEME=0"
        "-DUSE_LIBJPEG_TURBO=1"
        "-DENABLE_REMOTING=0"
        "-DENABLE_WEBRTC=0"
        "-DENABLE_EGLIMAGE=0"
        "-DENABLE_TASK_MANAGER=0"
        "-DENABLE_EXTENSIONS=0"
        "-DENABLE_PLUGIN_INSTALLATION=0"
        "-DENABLE_PLUGINS=1"
        "-DENABLE_SESSION_SERVICE=0"
        "-DENABLE_THEMES=0"
        "-DENABLE_AUTOFILL_DIALOG=0"
        "-DENABLE_BACKGROUND=0"
        "-DENABLE_AUTOMATION=0"
        "-DENABLE_GOOGLE_NOW=0"
        "-DENABLE_FULL_PRINTING=0"
        "-DENABLE_PRINTING=0"
        "-DENABLE_SPELLCHECK=0"
        "-DENABLE_CAPTIVE_PORTAL_DETECTION=0"
        "-DENABLE_APP_LIST=0"
        "-DENABLE_SETTINGS_APP=0"
        "-DENABLE_MANAGED_USERS=0"
        "-DENABLE_MDNS=1"
        "-DCLD_VERSION=2"
        "-DICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_STATIC"
        "-DDYNAMIC_ANNOTATIONS_ENABLED=1"
        "-DWTF_USE_DYNAMIC_ANNOTATIONS=1"
        "-D__STDC_CONSTANT_MACROS"
        "-D__STDC_FORMAT_MACROS"
    )
    target_include_directories (${PROJECT_NAME} PRIVATE ${INCLUDE_MAIN_LIST})
    target_sources (${PROJECT_NAME} PRIVATE ${SRC_MAIN_LIST})
endif ()

if (WIN32)
    target_compile_options (${PROJECT_NAME} PRIVATE
        "-DNOMINMAX"
        "-DPSAPI_VERSION=1"
        "-D_CRT_RAND_S"
        "-DCERT_CHAIN_PARA_HAS_EXTRA_FIELDS"
        "-D_ATL_NO_OPENGL"
        "-D_SECURE_ATL"
        "-DTOOLKIT_VIEWS=1"
        "-DUSE_AURA=0"
        "-DUSE_ASH=0"
        "-D_CRT_SECURE_NO_DEPRECATE"
        "-D_SCL_SECURE_NO_DEPRECATE"
        "-DNTDDI_VERSION=0x06020000"
    )
endif ()

if (LINUX)
    find_package (PkgConfig REQUIRED)
    pkg_check_modules (GTK REQUIRED gtk+-2.0 gtkglext-1.0)
    target_link_libraries (${PROJECT_NAME} ${GTK_LIBRARIES})
    target_include_directories (${PROJECT_NAME} PRIVATE ${GTK_INCLUDE_DIRS})
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        ${GTK_CLFLAGS_OTHER}
        # "`pkg-config --cflags gtk+-2.0 gtkglext-1.0`"
        "-DUSE_CAIRO=0"
        "-DUSE_GLIB=0"
        "-DUSE_NSS=1"
        "-DUSE_X11=0"
        "-DGTK_DISABLE_SINGLE_INCLUDES=1"
        "-DUSE_XI2_MT=2"
    )
    file (GLOB INCLUDE_LIST "${THIRD_PARTY_PATH}")
    target_include_directories (${PROJECT_NAME} PRIVATE ${INCLUDE_LIST})
endif ()

if (EMSCRIPTEN)
    file (
        GLOB_RECURSE
        SRC_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/emscripten/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/emscripten/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/emscripten/*.hpp"
    )
    target_sources (${PROJECT_NAME} PRIVATE ${SRC_LIST})
endif ()

if (APPLE)
    file (GLOB INCLUDE_APPLE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/lib/WebViewJavascriptBridge")
    target_compile_options (${PROJECT_NAME} PRIVATE -x objective-c++)
    file (
        GLOB_RECURSE
        SRC_APPLE_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/apple/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/apple/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/WebViewJavascriptBridge/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/WebViewJavascriptBridge/*.m"
    )
    target_include_directories (${PROJECT_NAME} PRIVATE ${INCLUDE_APPLE_LIST})
    target_sources (${PROJECT_NAME} PRIVATE ${SRC_APPLE_LIST})
endif ()

if (IOS)
    file (
        GLOB
        SRC_APPLE_LIST
        "include/apple/dom/IOSWebView.h"
        "src/apple/dom/IOSWebView.m"
    )
    target_sources (${PROJECT_NAME} PRIVATE ${SRC_APPLE_LIST})
endif ()

if (APPLE AND NOT IOS)
    file (
        GLOB
        SRC_APPLE_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/apple/dom/OSXWebView.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/apple/dom/OSXWebUIDelegate.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/apple/dom/OSXWebView.m"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/apple/dom/OSXWebUIDelegate.m"
    )
    # lib link might change
    target_link_libraries(${PROJECT_NAME} "-framework WebKit")
    target_sources (${PROJECT_NAME} PRIVATE ${SRC_APPLE_LIST})
endif ()

if (ANDROID)
    file (
        GLOB_RECURSE
        SRC_ANDROID_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/android/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/android/*.cpp"
    )
    file (
        GLOB 
        INCLUDE_ANDROID_LIST 
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
        )
    target_include_directories (${PROJECT_NAME} PRIVATE ${INCLUDE_ANDROID_LIST})
    target_sources (${PROJECT_NAME} PRIVATE "${SRC_ANDROID_LIST}")
    target_compile_options (${PROJECT_NAME} PRIVATE "-DJSON_IS_AMALGAMATION")
endif ()
