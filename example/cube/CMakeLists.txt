cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-cube")

config_output_path ()

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "asset/*"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "src"
)

list (APPEND
    ${PROJECT_NAME}_PLUGINS
    sdl
    png
)

add_executable (${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

target_include_directories (${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE} ${FRAMEWORK_INCLUDES})

foreach (${PROJECT_NAME}_PLUGIN ${${PROJECT_NAME}_PLUGINS})
    call_plugin(enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
endforeach()
# do function
target_link_libraries(${PROJECT_NAME} "minko-framework")
link_directories("/usr/lib64")
target_link_libraries(${PROJECT_NAME} "GL" "m" "pthread")

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    package_assets(".png;.glsl;.effect" "on")
    package_assets(".png;.glsl;.effect" "off")
elseif()
    package_assets(".png;.glsl;.effect" "off")
endif()