cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-example-cube")
function(call _id)
    if (NOT COMMAND ${_id})
        message(FATAL_ERROR "Unsupported function/macro \"${_id}\"")
    else()
        set(_helper "${CMAKE_BINARY_DIR}/helpers/macro_helper_${_id}.cmake")
        if (NOT EXISTS "${_helper}")
            file(WRITE "${_helper}" "${_id}(\$\{ARGN\})\n")
        endif()
        include("${_helper}")
    endif()
endfunction()
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

#foreach (${PROJECT_NAME}_PLUGIN IN_LISTS ${PROJECT_NAME}_PLUGINS)
    enable_sdl(${PROJECT_NAME})
    enable_png(${PROJECT_NAME})
#endforeach()
#do function
target_link_libraries(${PROJECT_NAME} "minko-framework")
link_directories("/usr/lib64")
target_link_libraries(${PROJECT_NAME} "GL" "m" "pthread")

package_assets(".png;.glsl;.effect")