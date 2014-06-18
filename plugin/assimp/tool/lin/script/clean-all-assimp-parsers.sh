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

source assimp-parser-decl.sh

for loader in ${!loaders[@]}; do
    parser_name=${loader}

    header_file=${include_path}/${parser_name}${minko_class_suffix}.hpp
    src_file=${src_path}/${parser_name}${minko_class_suffix}.cpp

    rm -fv ${header_file} ${src_file}
done

echo "all files successfully removed"
