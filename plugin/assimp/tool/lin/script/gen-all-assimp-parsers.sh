#!/bin/bash

if [ -z ${MINKO_HOME} ]; then
	echo "MINKO_HOME variable is empty"
	exit 1
fi

include_path=${MINKO_HOME}/plugin/assimp/include/minko/file
src_path=${MINKO_HOME}/plugin/assimp/src/minko/file

echo "include path: ${include_path}"
echo "src path: ${src_path}"

cd ${MINKO_HOME}/plugin/assimp/tool/lin/script/assimp-parser-generator

./gen-all-assimp-parsers.sh ${include_path} ${src_path}

./gen-assimp-plugin-main-header.sh
