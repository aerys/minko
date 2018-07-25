function (package_assets extensions embed)
    
    foreach (OBJ IN LISTS extensions)
        file (GLOB_RECURSE
            ASSETS_${OBJ}
            "${MINKO_HOME}/framework/asset/*${OBJ}"
            "${CMAKE_CURRENT_SOURCE_DIR}/asset/*${OBJ}"
        )
    endforeach()

    foreach (OBJ IN LISTS extensions)
        foreach (SUB_OBJ IN LISTS ASSETS_${OBJ})
        string (FIND ${SUB_OBJ} "asset/" DIR_SIZE)
        string (LENGTH ${SUB_OBJ} LEN)
        math (EXPR RESULT ${LEN}-${DIR_SIZE})
        string (SUBSTRING ${SUB_OBJ} ${DIR_SIZE} ${RESULT} NEW_DIR)
        if (embed STREQUAL "on")
            string (CONCAT FINAL_DIR "embed/" ${NEW_DIR})
        else()
            set (FINAL_DIR ${NEW_DIR})
        endif()
            configure_file ("${SUB_OBJ}"
                "${OUTPUT_PATH}/${FINAL_DIR}"
                COPYONLY
            )
        endforeach()
    endforeach()
endfunction()