function (copy source dest)

    file (GLOB_RECURSE
        SOURCE
        "${source}"
    )
    if (dest)
        set (DEST ${dest})
    elseif ()
        set (DEST ${CMAKE_CURRENT_SOURCE_DIR})
    endif ()
    foreach (OBJ IN LISTS SOURCE)
        configure_file ("${OBJ}"
            "${DEST}"
            COPYONLY
        )
    endforeach ()
endfunction ()