#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
OS_TYPE=`uname -s`

export OSTYPE=${OS_TYPE}

pushd ${DIR}/..
case ${OS_TYPE} in
	Linux)
		${MINKO_HOME}/script/premake5.sh $@ --cc=gcc gmake
		;;
	Darwin)
		${MINKO_HOME}/script/premake5.sh $@ --cc=gcc gmake
		;;
	CYGWIN*)
		${MINKO_HOME}/script/premake5.bat $@ --cc=gcc gmake
		;;
esac
popd
