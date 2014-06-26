#!/bin/bash

BIN="astyle"
OPTIONS="-rn -NYwcfpH -xd -s2 -k1 -W1 -z2 --style=allman"
PROJECTS="framework plugin/*"
FILES="src/*.cpp src/*.hpp include/*.hpp"

for PROJECT in $PROJECTS ; do
    pushd ${PROJECT}/${DIR}
    ${BIN} ${OPTIONS} ${FILES}
    popd
done
