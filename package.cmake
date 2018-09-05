list (
    APPEND
    MINKO_PACKAGES_DIRS
    "${MINKO_HOME}/framework/asset"
)

function (minko_package_assets extensions type)
    if (${type} STREQUAL "embed")
        minko_package_assets ("${extensions}" "copy")
    endif ()
    list (
        APPEND
        MINKO_PACKAGES_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/asset"
    )
    foreach (OBJ ${extensions})
        foreach (ASSET_DIR ${MINKO_PACKAGES_DIRS})
            file (
                GLOB_RECURSE
                ASSETS_TEMP
                "${ASSET_DIR}/${OBJ}"
            )
            list (
                APPEND
                ASSETS
                ${ASSETS_TEMP}
            )
        endforeach()
    endforeach ()

    foreach (SUB_OBJ IN LISTS ASSETS)
        string (FIND ${SUB_OBJ} "asset/" DIR_SIZE)
        string (LENGTH ${SUB_OBJ} LEN)
        math (EXPR RESULT ${LEN}-${DIR_SIZE})
        string (SUBSTRING ${SUB_OBJ} ${DIR_SIZE} ${RESULT} NEW_DIR)
        if (${type} STREQUAL "embed")
            string (CONCAT NEW_DIR "embed/" ${NEW_DIR})
        endif ()
        set (FINAL_DIR ${NEW_DIR})
        configure_file ("${SUB_OBJ}" "${OUTPUT_PATH}/${FINAL_DIR}" COPYONLY)
    endforeach ()
    unset (ASSETS)
endfunction ()