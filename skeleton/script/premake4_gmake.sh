#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
	$MINKO_HOME/tools/mac/bin/premake4 --platform=gcc gmake
else
	$MINKO_HOME/tools/lin/bin/premake4.sh --platform=gcc gmake
fi
