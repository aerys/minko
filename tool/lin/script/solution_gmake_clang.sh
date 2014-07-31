#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/../../.. > /dev/null
tool/lin/script/premake5.sh --no-test --cc=clang gmake
popd > /dev/null
