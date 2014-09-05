#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/../../.. > /dev/null
tool/lin/script/premake5.sh --rebuild-sdl --no-test --cc=gcc gmake
popd > /dev/null
