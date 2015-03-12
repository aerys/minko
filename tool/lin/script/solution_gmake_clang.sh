#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/../../.. > /dev/null
tool/lin/bin/premake5.sh $@ --cc=clang gmake
popd > /dev/null
