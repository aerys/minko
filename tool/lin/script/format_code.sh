#!/bin/bash

ASTYLE="astyle"
SED="sed"

ASTYLE_OPTIONS="--style=allman --indent=spaces=4 --convert-tabs --break-blocks --unpad-paren --pad-oper --pad-header --close-templates --indent-preproc-define --indent-namespaces --indent-col1-comments --align-pointer=type --align-reference=type --lineend=linux"
MODE_OPTIONS="--suffix=none --recursive"
PROJECTS="framework plugin/*"
FILES="src/*.cpp src/*.hpp include/*.hpp"

for PROJECT in $PROJECTS ; do
    pushd ${PROJECT}/${DIR} > /dev/null
    ${ASTYLE} ${MODE_OPTIONS} ${STYLE_OPTIONS} ${FILES}
    ${SED} -i'' -e 's/[ \t]*$//' ${FILES}
    popd > /dev/null
done
