function (enable_assimp target)

set (ASSIMP_PATH "${MINKO_HOME}/plugin/assimp")
list (APPEND
    ${PROJECT_NAME}_PLUGINS_ASSIMP
    zlib
)

foreach (${PROJECT_NAME}_PLUGIN ${${PROJECT_NAME}_PLUGINS_ASSIMP})
    call_plugin (enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
endforeach ()
plugin_link ("assimp", ${target})
target_include_directories(${target} PUBLIC "${ASSIMP_PATH}/include")
target_compile_options(${target} PUBLIC -DMINKO_PLUGIN_ASSIMP)
endfunction ()