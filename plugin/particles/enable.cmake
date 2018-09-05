function (minko_enable_plugin_particules target)
    set (PARTICULES_PATH "${MINKO_HOME}/plugin/particules")

    target_compile_options (${PROJECT_NAME} PRIVATE "-DMINKO_PLUGIN_PARTICLES")
    minko_plugin_link("particules" ${target})

    minko_enable_plugin_serializer (${target})

    file (GLOB PARTICULES_INCLUDE "${PARTICULES_PATH}/include")
    target_include_directories(${target} PRIVATE ${PARTICULES_INCLUDE})

    minko_copy (${PARTICULES_PATH}/asset ${OUTPUT_PATH} ${target})
endfunction ()