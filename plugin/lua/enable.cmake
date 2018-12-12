function (minko_enable_plugin_lua target)
    set (LUA_PATH "${MINKO_HOME}/plugin/lua")

    target_compile_options (${target} PRIVATE
        "-DMINKO_PLUGIN_LUA"
        "-DLUA_USE_POSIX"
    )

    file (GLOB
        ${PROJECT_NAME}_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/LuaGlue/include"
    )

    minko_plugin_link ("lua" ${target})
    file (COPY ${LUA_PATH}/asset DESTINATION ${OUTPUT_PATH}/asset)

    # function minko.plugin.lua:dist(pluginDistDir)
endfunction ()