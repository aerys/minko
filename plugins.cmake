function (plugin_link names target)
# add specifications see : module/minko/minko.plugin.lua
    foreach(name IN_LISTS names)
        target_link_libraries(${target} minko-plugin-${name})
    endforeach()
endfunction ()