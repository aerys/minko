#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
OS_TYPE=`uname -s`

export OSTYPE=${OS_TYPE}

pushd ${DIR}/..
case ${OS_TYPE} in
	Linux)
		${MINKO_HOME}/tool/lin/script/premake5.sh $@ --cc=gcc gmake
		;;
	Darwin)
		${MINKO_HOME}/tool/mac/script/premake5.sh $@ --cc=gcc gmake
		;;
	CYGWIN*)
		${MINKO_HOME}/tool/win/bin/premake5.exe $@ --cc=gcc gmake
		;;
esac
popd