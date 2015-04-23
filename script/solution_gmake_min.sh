#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd ${DIR}/.. > /dev/null
script/premake5.sh --no-test --no-example --no-tutorial $@ gmake
popd > /dev/null
