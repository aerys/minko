#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/../../..
tools/lin/scripts/premake5.sh --no-tests --cc=clang gmake
popd
