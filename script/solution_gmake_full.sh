#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd ${DIR}/.. > /dev/null
script/premake5.sh $@ gmake
popd > /dev/null
