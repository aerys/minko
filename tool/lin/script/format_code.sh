#!/bin/bash

ASTYLE="astyle"
SED="sed"

ASTYLE_OPTIONS="--style=allman --indent=spaces=4 --convert-tabs --break-blocks --unpad-paren --pad-oper --pad-header --close-templates --indent-preproc-define --indent-namespaces --indent-col1-comments --align-pointer=type --align-reference=type --lineend=linux"
MODE_OPTIONS="--suffix=none"
PROJECTS="framework plugin/* example/*"

for PROJECT in $PROJECTS ; do
	echo "============================="
	echo "Entering: ${PROJECT}"
	echo "============================="
    pushd ${PROJECT}/${DIR} > /dev/null
    FILES=$(find src include -iname "*.hpp" -or -iname "*.cpp")
    [[ -n "${FILES}" ]] && {
	    ${ASTYLE} ${MODE_OPTIONS} ${ASTYLE_OPTIONS} ${FILES}
    	${SED} -i'' -e 's/[ \t]*$//;s/2013 Aerys/2014 Aerys/' ${FILES}
    }
    popd > /dev/null
done
