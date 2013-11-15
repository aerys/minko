#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
	$MINKO_HOME/tools/mac/bin/premake4 gmake
else
	$MINKO_HOME/tools/lin/bin/premake4.sh gmake
fi
