# Fails if ARCHIVE references a `node::` symbol that LIBRARY does not export.
# Usage: cmake -D NM=<llvm-nm> -D ARCHIVE=<lib.a> -D LIBRARY=<libnode.so> -P check-node-symbols.cmake

# `list (FILTER)`, and lists that keep empty elements (CMP0007).
cmake_minimum_required (VERSION 3.10)

function (list_symbols output)
    execute_process (
        COMMAND "${NM}" --just-symbol-name ${ARGN}
        OUTPUT_VARIABLE SYMBOLS
        RESULT_VARIABLE RESULT
    )
    if (NOT RESULT EQUAL 0)
        message (FATAL_ERROR "${NM} ${ARGN} failed: ${RESULT}")
    endif ()

    string (REPLACE "\n" ";" SYMBOLS "${SYMBOLS}")
    set (${output} "${SYMBOLS}" PARENT_SCOPE)
endfunction ()

list_symbols (USED --undefined-only "${ARCHIVE}")
# Functions (`_ZN`), const member functions (`_ZNK`), vtables (`_ZTV`) and typeinfo (`_ZTI`).
list (FILTER USED INCLUDE REGEX "^_Z(T[VI])?NK?4node")
list (REMOVE_DUPLICATES USED)

# An empty list would check nothing (another nm output format, or a dropped call).
list (FIND USED "_ZN4node5StartEiPPc" INDEX)
if (INDEX EQUAL -1)
    message (FATAL_ERROR "${ARCHIVE} does not reference node::Start (_ZN4node5StartEiPPc) according to ${NM}.")
endif ()

list_symbols (EXPORTED --dynamic --defined-only "${LIBRARY}")

foreach (SYMBOL ${USED})
    list (FIND EXPORTED "${SYMBOL}" INDEX)
    if (INDEX EQUAL -1)
        message (FATAL_ERROR "${LIBRARY} does not export ${SYMBOL}, used by ${ARCHIVE}.")
    endif ()
endforeach ()
