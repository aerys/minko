#!/bin/bash

pushd ../../..
tools/lin/scripts/premake5.sh --no-tests --cc=gcc gmake
popd
