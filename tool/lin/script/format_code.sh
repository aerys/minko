#!/bin/bash

ASTYLE="astyle"
SED="sed"

PROJECTS="framework plugin/* example/*"

ASTYLE_OPTIONS="--suffix=none --style=allman --indent=spaces=4 --convert-tabs --break-blocks --unpad-paren --pad-oper --pad-header --close-templates --indent-preproc-define --indent-namespaces --indent-col1-comments --align-pointer=type --align-reference=type --lineend=linux"
SED_OPTIONS="
    s/[ \t]*$//                     ; # trailing white spaces
    s/\t/    /g                     ; # tab -> spaces
    s/2013 Aerys/2014 Aerys/        ; # copyright
"

for PROJECT in $PROJECTS ; do
    echo "============================="
    echo "Entering: ${PROJECT}"
    echo "============================="
    pushd ${PROJECT}/${DIR} > /dev/null
    FILES=$(find src include -iname "*.hpp" -or -iname "*.cpp")
    [[ -n "${FILES}" ]] && {
        # ${ASTYLE} ${ASTYLE_OPTIONS} ${FILES}
        # ${CLANGFORMAT} ${CLANGFORMAT_OPTIONS} ${FILES}
        ${SED} -i'' -e "${SED_OPTIONS}" ${FILES}
    }
    popd > /dev/null
done

