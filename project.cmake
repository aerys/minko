function (minko_add_library target_name type sources)  
    add_library (${target_name} ${type} ${sources})
    configure_target_flags (${target_name})
    set (OUTPUT_PATH ${OUTPUT_PATH} PARENT_SCOPE)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
    list (APPEND FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
    )
    target_include_directories(${target_name} PUBLIC ${FRAMEWORK_INCLUDES})
    if (NOT ${target_name} STREQUAL "minko-framework")
        target_link_libraries(${target_name}
            "minko-framework"
        )
    endif ()
    
    if (ANDROID)
        message("TON PERRRREEOEHNUIBNJE")
        add_custom_command(TARGET ${target_name}
            PRE_LINK
            COMMAND ${MINKO_HOME}/script/cpjf.sh ${CMAKE_CURRENT_SOURCE_DIR}/src ${OUTPUT_PATH}/src/com/minko
        )
        target_link_libraries(${target_name}
            "GLESv1_CM"
            "GLESv2"
            "EGL"
            "dl"
            "z"
            "log"
            "android"
            "stdc++"
        )
        set_target_properties (${target_name} PROPERTIES LINK_FLAGS 
            "-Wl -shared -pthread -Wl,--no-undefined -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit"
        )
        target_compile_options(${target_name} PUBLIC "-Wno-narrowing" "-Wno-tautological-compare" "-DEVL_EGLEXT_PROTOTYPES")
    endif ()
    
    if (WIN32)
        target_include_directories(${target_name}
            PUBLIC 
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
            "${MINKO_HOME}/framework/lib/glew/include"
        )
        target_compile_options(${target_name} PUBLIC "/wd4996")
    else ()
        target_include_directories(${target_name} PUBLIC "/framework/lib/sparsehash/include")
    endif ()

    if (IOS)
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 
            "7.0"
        )
    endif ()
    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target_name} PUBLIC "-DDEBUG")
    else ()
        target_compile_options(${target_name} PUBLIC "-DNDEBUG")
    endif ()

    if (MSVC)
        target_compile_options(${target_name}
            PUBLIC 
            "-DNOMINMAX" 
            "-D_VARIADIC_MAX=10" 
            "-D_USE_MATH_DEFINES" 
            "-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS"
            "/wd4503"
        )
    endif ()

    if (EMSCRIPTEN)
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target_name} PRIVATE
                -O3 
                --llvm-lto 1
            )
        else ()
            target_compile_options(${target_name} PRIVATE
                --llvm-opts 0
                -O2
                -g4
            )
        endif ()
    endif ()
endfunction ()


function (minko_add_executable target_name sources)
    add_executable (${target_name} ${sources})
    configure_target_flags (${target_name})
    set (OUTPUT_PATH ${OUTPUT_PATH} PARENT_SCOPE)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${target_name}
        "minko-framework"
    )
    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target_name} PUBLIC "-DDEBUG")
    else ()
        target_compile_options(${target_name} PUBLIC "-DNDEBUG")
    endif ()
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (BITNESS 64)
    else ()
        set (BITNESS 32)
    endif ()
    package_assets(".glsl;.effect" off)
    string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)
    link_directories("${MINKO_HOME}/framework/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}")
    if (WIN32)
        find_library(GLEW32_LIB glew32 HINTS "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}")
        target_link_libraries(${target_name}
            "minko-framework"
            "OpenGL32"
            ${GLEW32_LIB}
        )
        file (GLOB
            WINDOWS_DLL
            "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}/*.dll"
        )
        foreach (DLL ${WINDOWS_DLL})
            configure_file("${DLL}" "${OUTPUT_PATH}" COPYONLY)
        endforeach ()
    endif ()
    if (UNIX AND NOT APPLE AND NOT ANDROID)
        set (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl --no-as-needed")
        link_directories ("/usr/lib64")
        target_link_libraries(${target_name}
            "minko-framework"
            "GL"
            "m"
            "pthread"
        )
    elseif (APPLE AND NOT IOS)
        target_link_libraries(${target_name}
            "minko-framework"
            "m"
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
        )
    elseif (IOS)
        target_link_libraries(${target_name}
            "minko-framework"
            "m"
            "-framework OpenGLES"
            "-framework Foundation"
            "-framework UIKit"
            "-framework QuartzCore"
            "-framework CoreGraphics"
        )
        set_target_properties (${target_name} PROPERTIES XCODE_PRODUCT_TYPE com.apple.product-type.application)
        configure_file(${MINKO_HOME}/skeleton/Info.plist ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist COPYONLY)
        configure_file(${MINKO_HOME}/skeleton/Default-568h@2x.png ${CMAKE_CURRENT_SOURCE_DIR}/Default-568h@2x.png COPYONLY)
    endif ()
    if (EMSCRIPTEN)
        target_link_libraries(${target_name}
            "minko-framework"
        )
        if (WASM)
            set (NB_WASM 1)
        else ()
            set (NB_WASM 0)
        endif ()
        set_target_properties (${target_name} PROPERTIES LINK_FLAGS
        "-Wl --no-as-needed -s USE_SDL=2  -s FORCE_FILESYSTEM=1 -o ${OUTPUT_PATH}/${PROJECT_NAME}.bc -s WASM=${NB_WASM}")
        set_target_properties (${target_name} PROPERTIES SUFFIX ".bc")
        if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_custom_command(TARGET
                ${target_name}
                POST_BUILD
                COMMAND $ENV{EMSCRIPTEN}/emcc ${OUTPUT_PATH}/${PROJECT_NAME}.bc -o ${OUTPUT_PATH}/${PROJECT_NAME}.html --js-library ${MINKO_HOME}/module/emscripten/library.js --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -O3 --closure 1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -s USE_SDL=2 --preload-file ${OUTPUT_PATH}/embed/asset@asset -s WASM=${NB_WASM} #--shell-file \"${MINKO_HOME}/skeleton/template.html\"
            )
        else ()
            add_custom_command(TARGET
                ${target_name}
                POST_BUILD
                COMMAND $ENV{EMSCRIPTEN}/emcc ${OUTPUT_PATH}/${PROJECT_NAME}.bc -o ${OUTPUT_PATH}/${PROJECT_NAME}.html --js-library ${MINKO_HOME}/module/emscripten/library.js --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s DEMANGLE_SUPPORT=1  -s FORCE_FILESYSTEM=1 -s USE_SDL=2 --preload-file ${OUTPUT_PATH}/embed/asset@asset -s WASM=${NB_WASM} #--shell-file \"${MINKO_HOME}/skeleton/template.html\"
            )
        endif ()
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/template.html")
            target_compile_options(${target_name}
                PUBLIC
                 "--shell-file \"${CMAKE_CURRENT_SOURCE_DIR}/template.html\""
            )
        else ()
            target_compile_options(${target_name}
                PUBLIC
                "--shell-file \"${MINKO_HOME}/skeleton/template.html\""
            )
        endif ()
    endif ()
    
    if (IOS)
        file (GLOB_RECURSE 
            IOS_SRC
            "*.plist"
        )
        target_sources(${target_name} PUBLIC ${IOS_SRC})
    endif ()
endfunction ()