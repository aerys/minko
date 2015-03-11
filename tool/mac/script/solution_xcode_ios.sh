#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/../../.. > /dev/null
tool/mac/bin/premake5.sh $@ xcode-ios
popd > /dev/null
