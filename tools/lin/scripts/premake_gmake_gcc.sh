#!/bin/bash

pushd ../../..
tools/lin/bin/premake5.sh --no-tests --cc=gcc gmake
popd
