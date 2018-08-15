#!/bin/bash

set -x
test -d $1 || exit 1
cd $1

mkdir -p plugin/serializer/lib
pushd plugin/serializer/lib
test -d PVRTexTool || {
  ARCHIVE=smartshape-converter-lib
  curl -s http://static.aerys.in/smartshape/converter/lib.tgz --output ./${ARCHIVE}.tgz
  tar xzf ${ARCHIVE}.tgz
  rm -rf ./${ARCHIVE}.tgz
}
popd
