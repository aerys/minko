function (cpjf src_dir dest_dir target)
    file (
        GLOB_RECURSE
        JAVA_FILES
        "${src_dir}/*.java"
    )

    foreach (FILE ${JAVA_FILES})
        string (FIND ${FILE} "src/" DIR_SIZE)
        string (LENGTH ${FILE} LEN)
        math (EXPR RESULT ${LEN}-${DIR_SIZE}-4)
        math (EXPR DIR_SIZE ${DIR_SIZE}+4)
        string (SUBSTRING ${FILE} ${DIR_SIZE} ${RESULT} FINAL_DIR)
        configure_file ("${FILE}" "${dest_dir}/src/com/${FINAL_DIR}" COPYONLY)
    endforeach ()
endfunction()