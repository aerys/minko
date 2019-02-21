function (minko_enable_plugin_fx target)
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_FX")
    get_target_property (target_type ${target} TYPE)
    set (FX_PATH "${MINKO_HOME}/plugin/fx")
    
    minko_plugin_link ("fx" ${target})

    minko_package_assets(
        ${target}
        EMBED
        "${FX_PATH}/asset/effect/*/*.effect"
        "${FX_PATH}/asset/effect/*/*.glsl"
    )
endfunction ()