#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
	$MINKO_HOME/tools/mac/bin/premake5 gmake
else
	$MINKO_HOME/tools/lin/bin/premake5.sh gmake
fi
