cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-sensors")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

file (GLOB_RECURSE
    ${PROJECT_NAME}_EXCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/android/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/apple/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/android/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/apple/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/emscripten/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/emscripten/*"
)

foreach(${PROJECT_NAME}_EXCLUDE ${${PROJECT_NAME}_EXCLUDES})
    list(REMOVE_ITEM ${PROJECT_NAME}_SRC ${${PROJECT_NAME}_EXCLUDE})
endforeach()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_sdl (${PROJECT_NAME})

if (EMSCRIPTEN)
    file (GLOB_RECURSE
        ${PROJECT_NAME}_SRC_PLATFORM
        "${CMAKE_CURRENT_SOURCE_DIR}/include/emscripten/*"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/emscripten/*"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SRC_PLATFORM})
endif ()

if (ANDROID)
    file (GLOB_RECURSE
        ${PROJECT_NAME}_SRC_PLATFORM
        "${CMAKE_CURRENT_SOURCE_DIR}/include/android/*"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/android/*"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SRC_PLATFORM})
endif ()

if (APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -x objective-c++)
endif ()

if (IOS)
    file (GLOB_RECURSE
        ${PROJECT_NAME}_SRC_PLATFORM
        "${CMAKE_CURRENT_SOURCE_DIR}/include/apple/*"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/apple/*"
    )
    target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SRC_PLATFORM})
    target_link_libraries(${PROJECT_NAME} "-framework CoreMotion")
endif ()