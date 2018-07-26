function (copy source dest target)

    if (dest)
        set (DEST ${dest})
    elseif ()
        set (DEST ${CMAKE_CURRENT_SOURCE_DIR})
    endif ()
    add_custom_command(TARGET
        ${target}
        PRE_BUILD
        COMMAND cmake -E copy_directory ${source} ${dest}
    )
endfunction ()