### Loads DevIL 3rdParty library.
add_subdirectory(devil/DevIL)

### Silents some warnings when building DevIL's IL library.
target_compile_options(IL PRIVATE
    -Wno-unused-result
    -Wno-unused-value
    -Wno-invalid-source-encoding
    -Wno-tautological-pointer-compare
    -Wno-tautological-constant-out-of-range-compare
    -Wno-address-of-packed-member
    -Wno-sizeof-pointer-memaccess)