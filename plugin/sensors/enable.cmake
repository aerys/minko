function (minko_enable_plugin_sensors target)
    set (SENSORS_PATH "${MINKO_HOME}/plugin/sensors")
    minko_plugin_link ("sensors" ${target})
    target_include_directories(${target} 
        PRIVATE 
        "${SENSORS_PATH}/include"
    )
    target_compile_options(${target} 
        PRIVATE
        -DMINKO_PLUGIN_SENSORS
    )
    
    list (APPEND
        SENSORS_PLUGINS
        sdl
    )
    minko_enable_plugin_sdl (${target})
endfunction ()