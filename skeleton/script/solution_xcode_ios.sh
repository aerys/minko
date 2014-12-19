#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

pushd ${DIR}/.. > /dev/null
${MINKO_HOME}/tool/mac/script/premake5.sh xcode-ios
popd > /dev/null
