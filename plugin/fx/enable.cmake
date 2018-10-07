function (minko_enable_plugin_fx target)
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_FX")
    get_target_property (target_type ${target} TYPE)
    set (FX_PATH "${MINKO_HOME}/plugin/fx")
    if (NOT ${target_type} STREQUAL "STATIC_LIBRARY")
        minko_plugin_link ("fx" ${target})
        target_include_directories(${target} PRIVATE "${FX_PATH}/include")
        list(APPEND
            FX_ASSET_FOLDERS
            "AnamorphicLensFlare"
            "CelShading"
            "Depth"
            "FXAA"
            "Hologram"
            "LightScattering"
            "PseudoLensFlare"
            "Reflection"
            "Skybox"
            "Water"
        )

        foreach(FX_ASSET_FOLDER ${FX_ASSET_FOLDERS})        
            minko_copy ("${FX_PATH}/asset/effect/${FX_ASSET_FOLDER}" "${OUTPUT_PATH}/asset/effect/${FX_ASSET_FOLDER}" ${target})
        endforeach()
        list (
            APPEND
            MINKO_PACKAGES_DIRS
            "${FX_PATH}/asset"
        )
        set (MINKO_PACKAGES_DIRS ${MINKO_PACKAGES_DIRS} PARENT_SCOPE)
    endif ()
endfunction ()