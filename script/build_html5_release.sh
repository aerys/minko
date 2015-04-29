#!/bin/bash

CONFIG=html5_release

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd ${DIR}/.. > /dev/null
make config=${CONFIG} verbose=1 -j4 $@
popd > /dev/null
