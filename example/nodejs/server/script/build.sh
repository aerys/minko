#!/bin/bash

set -e
set -x

npm install
webpack
cp package.json bin
pushd bin
npm install --production
rm -f package.json
rm -f ../../asset/server.zip
zip -r ../../asset/server.zip .
popd
