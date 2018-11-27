function (minko_add_library target_name type sources)
    minko_set_variables()
    add_library (${target_name} ${type} ${sources})
    minko_configure_target_flags (${target_name})
    set (OUTPUT_PATH ${OUTPUT_PATH} PARENT_SCOPE)
    set (BITNESS ${BITNESS} PARENT_SCOPE)
    set (BUILD_TYPE ${BUILD_TYPE} PARENT_SCOPE)
    set (SYSTEM_NAME ${SYSTEM_NAME} PARENT_SCOPE)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
    
    if (APPLE)
        target_compile_options(${target_name} PUBLIC -stdlib=libc++)
    endif ()

    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options (${target_name} PUBLIC "-DDEBUG")
    else ()
        target_compile_options (${target_name} PUBLIC "-DNDEBUG")
    endif ()
    
    list (APPEND FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
    )
    if (WIN32)
        list (
            APPEND
            FRAMEWORK_INCLUDES
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
        )
    else ()
        list (
            APPEND
            FRAMEWORK_INCLUDES
            "${MINKO_HOME}/framework/lib/sparsehash/include"
        )
    endif ()   
    target_include_directories (${target_name} PRIVATE "${FRAMEWORK_INCLUDES}")
    
    string (FIND ${target_name} "minko-example" TEST_EXAMPLE)
    string (FIND ${target_name} "minko-plugin" TEST_PLUGIN)
    string (FIND ${target_name} "minko-framework" TEST_FRAMEWORK)
    string (FIND ${target_name} "libassimp" TEST_LIBASSIMP)
    
    if (TEST_EXAMPLE EQUAL -1 AND TEST_PLUGIN EQUAL -1 AND TEST_FRAMEWORK EQUAL -1 AND TEST_LIBASSIMP EQUAL -1)
        if (NOT EMSCRIPTEN AND NOT ANDROID)
            find_library(
                MINKO_FRAMEWORK_LIB 
                NAMES minko-framework
                HINTS "${MINKO_HOME}/build/framework/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
            )
        else()
            set (MINKO_FRAMEWORK_LIB "${MINKO_HOME}/build/framework/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}/libminko-framework.a")
        endif ()
    else ()
        set (MINKO_FRAMEWORK_LIB "minko-framework")
    endif ()
    
    if (TEST_FRAMEWORK EQUAL -1)
        target_link_libraries(${target_name} ${MINKO_FRAMEWORK_LIB})
    endif ()
    
    if (UNIX AND NOT APPLE AND NOT ANDROID)
        target_link_libraries (${target_name} "-lGL")
    endif ()
    if (ANDROID)
        add_custom_command (
            TARGET ${target_name}
            PRE_LINK
            COMMAND ${MINKO_HOME}/script/cpjf.sh ${CMAKE_CURRENT_SOURCE_DIR}/src ${OUTPUT_PATH}/src/com/minko
        )
        target_link_libraries (${target_name}
            "GLESv1_CM"
            "GLESv2"
            "EGL"
            "dl"
            "z"
            "log"
            "android"
            "stdc++"
        )
        set_target_properties (
            ${target_name}
            PROPERTIES LINK_FLAGS 
            "-Wl -shared -pthread -Wl,--no-undefined -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit"
        )
        target_compile_options (${target_name} PUBLIC "-Wno-narrowing" "-Wno-tautological-compare" "-DEVL_EGLEXT_PROTOTYPES")
    endif ()
    
    if (WIN32)
        target_include_directories (${target_name}
            PUBLIC 
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
            "${MINKO_HOME}/framework/lib/glew/include"
        )
        target_compile_options (${target_name} PUBLIC "/wd4996")
    else ()
        target_include_directories (${target_name} PUBLIC "/framework/lib/sparsehash/include")
    endif ()

    if (IOS)
        set_target_properties (
            ${target_name}
            PROPERTIES
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 
            "7.0"
        )
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
            target_compile_options(${target_name} PRIVATE -O3 --llvm-lto 1)
        else ()
            target_compile_options(${target_name} PRIVATE --llvm-opts 0 -O2 -g4)
        endif ()
    endif ()
    
    string (FIND ${target_name} "minko-plugin" TEST_PLUGIN)
    
    if (ANDROID AND TEST_PLUGIN EQUAL -1)
        foreach (ANDROID_PLUGIN_JAVA_SRC ${${PROJECT_NAME}_PLUGINS})
            add_custom_command (
                TARGET ${target_name}
                PRE_LINK
                COMMAND ${MINKO_HOME}/script/cpjf.sh ${MINKO_HOME}/plugin/${ANDROID_PLUGIN_JAVA_SRC}/src src/minko/com
                WORKING_DIRECTORY ${OUTPUT_PATH}
            )
        endforeach()

        minko_package_assets ("*.glsl;*.effect" "embed")
        build_android (${target_name} "lib${target_name}.so")
    endif ()
endfunction ()

# minko_add_executable function start
function (minko_add_executable target_name sources)
    minko_set_variables()
    string (FIND ${target_name} "minko-framework" TEST_FRAMEWORK)
    string (FIND ${target_name} "libassimp" TEST_LIBASSIMP)

    if (TEST_FRAMEWORK EQUAL -1 AND TEST_LIBASSIMP EQUAL -1)
        if (NOT EMSCRIPTEN AND NOT ANDROID)
            find_library(
                MINKO_FRAMEWORK_LIB 
                NAMES minko-framework
                HINTS "${MINKO_HOME}/build/framework/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
            )
        else()
            set (MINKO_FRAMEWORK_LIB "${MINKO_HOME}/build/framework/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}/libminko-framework.a")
        endif ()
    endif ()

    message(${MINKO_FRAMEWORK_LIB})
    set(CMAKE_CXX_STANDARD_LIBRARIES ${MINKO_FRAMEWORK_LIB} PARENT_SCOPE)
    add_executable (${target_name} ${sources})
    minko_configure_target_flags (${target_name})

    if (WITH_OFFSCREEN STREQUAL "on" OR WITH_OFFSCREEN STREQUAL "ON")
        minko_enable_plugin_offscreen (${target_name})
    endif ()

    set (OUTPUT_PATH ${OUTPUT_PATH} PARENT_SCOPE)
    set (BITNESS ${BITNESS} PARENT_SCOPE)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set (SYSTEM_NAME ${SYSTEM_NAME} PARENT_SCOPE)
    set (BUILD_TYPE ${BUILD_TYPE} PARENT_SCOPE)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
    
    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options (${target_name} PUBLIC "-DDEBUG")
    else ()
        target_compile_options (${target_name} PUBLIC "-DNDEBUG")
    endif ()
    
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (BITNESS 64)
    else ()
        set (BITNESS 32)
    endif ()

    list (
        APPEND
        FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
    )
    
    if (WIN32)
        list (
            APPEND
            FRAMEWORK_INCLUDES
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
        )
    else ()
        list (
            APPEND
            FRAMEWORK_INCLUDES
            "${MINKO_HOME}/framework/lib/sparsehash/include"
    )    
    endif ()
    
    minko_package_assets ("*.glsl;*.effect;*.png" "embed")
    target_include_directories (${target_name} PRIVATE "${FRAMEWORK_INCLUDES}")
    
    if (WIN32)
        find_library (GLEW32_LIB glew32 HINTS "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}")
        target_link_libraries (
            ${target_name}
            ${MINKO_FRAMEWORK_LIB}
            "OpenGL32"
            ${GLEW32_LIB}
        )
        file (
            GLOB
            WINDOWS_DLL
            "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}/*.dll"
        )
        foreach (DLL ${WINDOWS_DLL})
            configure_file ("${DLL}" "${OUTPUT_PATH}" COPYONLY)
        endforeach ()
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
    
    if (UNIX AND NOT APPLE AND NOT ANDROID)
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl --no-as-needed")
        link_directories ("/usr/lib64")
        target_link_libraries (
            ${target_name}
            ${MINKO_FRAMEWORK_LIB}
            "GL"
            "m"
            "-pthread"
        )
    elseif (APPLE AND NOT IOS)
        target_link_libraries (
            ${target_name}
            ${MINKO_FRAMEWORK_LIB}
            "m"
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
        )
    elseif (IOS)
        target_link_libraries (
            ${target_name}
            ${MINKO_FRAMEWORK_LIB}
            "m"
            "-framework OpenGLES"
            "-framework Foundation"
            "-framework UIKit"
            "-framework QuartzCore"
            "-framework CoreGraphics"
        )
        set_target_properties (${target_name} PROPERTIES XCODE_PRODUCT_TYPE com.apple.product-type.application)
        configure_file (${MINKO_HOME}/skeleton/Info.plist ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist COPYONLY)
        configure_file (${MINKO_HOME}/skeleton/Default-568h@2x.png ${CMAKE_CURRENT_SOURCE_DIR}/Default-568h@2x.png COPYONLY)
    endif ()
    
    if (APPLE)
        target_compile_options (${target_name} PUBLIC "-std=c++11")
    endif ()
    
    if (EMSCRIPTEN)
        if (WASM)
            set (WASM_NBR 1)
            set (WEB_TARGET wasm)
        else ()
            set (WASM_NBR 0)
            set (WEB_TARGET asmjs)
        endif ()
        set (WEB_TARGET ${WEB_TARGET} PARENT_SCOPE)
        
        set_target_properties (
            ${target_name}
            PROPERTIES LINK_FLAGS
            "-Wl --no-as-needed -s FORCE_FILESYSTEM=1 -o ${OUTPUT_PATH}/${PROJECT_NAME}.bc -s WASM=${WASM_NBR}"
        )
        set_target_properties (${target_name} PROPERTIES SUFFIX ".bc")
        
        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/template.html)
            set (SHELL_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/template.html)
        else ()
            set (SHELL_FILE_PATH ${MINKO_HOME}/skeleton/template.html)
        endif ()
        
        configure_file (${SHELL_FILE_PATH} ${OUTPUT_PATH}/template.html COPYONLY)
        set (${SHELL_FILE_PATH} ${OUTPUT_PATH}/template.html)

        # Step 1: Generate project-name.hmtl
        # Fill the template.html {{{ SCRIPT }}} field
        # Rename template.html
        add_custom_command (
            TARGET ${target_name}
            PRE_LINK
            COMMAND python ${MINKO_HOME}/adapt-template.py ${PROJECT_NAME} ${OUTPUT_PATH}/template.html
            COMMAND cp ${OUTPUT_PATH}/template.html ${OUTPUT_PATH}/${PROJECT_NAME}.html
        )

        # Step 2: call CMake build with WASM target enabled (create the child process)
        # The call is made after the ASM.js target build but before the ASM.js html project files generation
        if (WASM STREQUAL "off" OR WASM STREQUAL "OFF" AND EMSCRIPTEN)
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                COMMAND cd ${CMAKE_SOURCE_DIR} && mkdir -p build && cd build && cmake -DWASM=ON .. && make VERBOSE=1
            )
        endif ()

        # Step 3: generate the project files from the bytecode file for the WASM target
        # Note that Step 3 is within the child process, therefore it will generate the WASM project files
        # Step 4: generate the project files from the bytecode file for the ASM.js target
        # Note that Step 4 is within the parent process
        # At this point the target is back to ASM.js, therefore it will generate the ASM.js project files
        if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
            set_target_properties (
                ${target_name}
                PROPERTIES LINK_FLAGS
                "--llvm-lto 0"
            )
            # Files generation begin
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                COMMAND $ENV{EMSCRIPTEN}/em++ ${OUTPUT_PATH}/${PROJECT_NAME}.bc -o ${OUTPUT_PATH}/${PROJECT_NAME}-${WEB_TARGET}.html --js-library ${MINKO_HOME}/module/emscripten/library.js -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=${WASM_NBR} -s DEMANGLE_SUPPORT=1 -s NO_EXIT_RUNTIME=1 --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -s FORCE_FILESYSTEM=1 -s WASM=${WASM_NBR} --shell-file \"${SHELL_FILE_PATH}\"
                COMMAND python ${MINKO_HOME}/module/emscripten/empkg.py ${OUTPUT_PATH}/${PROJECT_NAME}-${WEB_TARGET}.data
            )
            # Files generation end
        else ()
            set_target_properties (
                ${target_name}
                PROPERTIES LINK_FLAGS
                "--llvm-lto 1"
            )
            # Files generation begin
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                COMMAND $ENV{EMSCRIPTEN}/em++ ${OUTPUT_PATH}/${PROJECT_NAME}.bc -o ${OUTPUT_PATH}/${PROJECT_NAME}-${WEB_TARGET}.html -O3 --closure 1 --js-library ${MINKO_HOME}/module/emscripten/library.js -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH=${WASM_NBR} -s NO_EXIT_RUNTIME=1 --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -s FORCE_FILESYSTEM=1 -s WASM=${WASM_NBR} -s OUTLINING_LIMIT=20000 --shell-file \"${SHELL_FILE_PATH}\"
                COMMAND python ${MINKO_HOME}/module/emscripten/empkg.py ${OUTPUT_PATH}/${PROJECT_NAME}-${WEB_TARGET}.data
            )
            # Files generation end
        endif ()

        # Step 5: clean output directories
        # The CMake variable 'WASM' is set to OFF only within the parent process
        # Therefore, the following code will only run when generation is over for both WASM and ASM.js targets
        if (WASM STREQUAL "off" OR WASM STREQUAL "OFF" AND EMSCRIPTEN)
            # Copy files from the ASM.js output directory to the WASM output directory
            # Remove bytecode file, template file and target-specific html files
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                COMMAND cp ${OUTPUT_PATH}/${PROJECT_NAME}-${WEB_TARGET}.* ${OUTPUT_PATH}/../../wasm${BITNESS}/${BUILD_TYPE}
                COMMAND rm -f ${OUTPUT_PATH}/../../wasm${BITNESS}/${BUILD_TYPE}/${PROJECT_NAME}.bc
                COMMAND rm -f ${OUTPUT_PATH}/../../wasm${BITNESS}/${BUILD_TYPE}/${PROJECT_NAME}-*.html
                COMMAND rm -f ${OUTPUT_PATH}/../../wasm${BITNESS}/${BUILD_TYPE}/template.html
            )
            # Change CMake output path for external projects
            string (REPLACE "asmjs${BITNESS}/" "wasm${BITNESS}/" NEW_PATH ${OUTPUT_PATH})
            set (OUTPUT_PATH ${NEW_PATH} PARENT_SCOPE)
            set (OUTPUT_PATH ${NEW_PATH})
            # Remove ASM.js output directory
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                COMMAND cmake -E remove_directory ${OUTPUT_PATH}/../../asmjs${BITNESS}
            )
        endif ()

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/template.html")
            target_compile_options (
                ${target_name}
                PUBLIC
                 "--shell-file \"${CMAKE_CURRENT_SOURCE_DIR}/template.html\""
            )
        else ()
            target_compile_options (
                ${target_name}
                PUBLIC
                "--shell-file \"${MINKO_HOME}/skeleton/template.html\""
            )
        endif ()
    endif ()
    
    if (IOS)
        file (GLOB_RECURSE IOS_SRC "*.plist")
        target_sources(${target_name} PUBLIC ${IOS_SRC})
    endif ()
endfunction ()
# minko_add_executable function end

function (minko_add_worker target_name sources)
    message(${sources})
    minko_add_library (${target_name} "STATIC" "${sources}")

    get_property (
        WORKER_LIBS 
        TARGET ${target_name} 
        PROPERTY LINK_LIBRARIES
    )
    list (
        REMOVE_ITEM
        WORKER_LIBS
        "minko-framework"
    )

    if (EMSCRIPTEN)
        add_custom_command (
            TARGET ${target_name}
            POST_BUILD
            COMMAND $ENV{EMSCRIPTEN}/emcc ${OUTPUT_PATH}/${PROJECT_NAME} -o ${OUTPUT_PATH}/${PROJECT_NAME}.js -O3 --closure 1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 -s EXPORTED_FUNCTIONS="[\'minkoWorkerEntryPoint\']"
        )       
    endif ()
endfunction ()