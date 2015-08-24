#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
OS_TYPE=`uname -s`

pushd ${DIR}/.. > /dev/null
case ${OS_TYPE} in
	Linux)
		${MINKO_HOME}/script/premake5.sh --cc=clang gmake
		;;
	Darwin)
		${MINKO_HOME}/script/premake5.sh --cc=clang gmake
		;;
	CYGWIN*)
		${MINKO_HOME}/script/premake5.bat $@ --cc=gcc gmake
		;;
esac
popd > /dev/null
