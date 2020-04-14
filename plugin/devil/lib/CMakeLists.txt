### Loads DevIL 3rdParty library.
add_subdirectory(devil/DevIL)

### Silents some warnings when building DevIL's IL library.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
    CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR
    EMSCRIPTEN)
    target_compile_options(IL PRIVATE
        -Wno-unused-result
        -Wno-unused-value
        -Wno-invalid-source-encoding
        -Wno-tautological-pointer-compare
        -Wno-tautological-constant-out-of-range-compare
        -Wno-address-of-packed-member
        -Wno-sizeof-pointer-memaccess)
endif ()