#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/../../..
tool/mac/script/premake5.sh --no-test xcode
popd
