#!/bin/bash

function print_usage
{
    echo -e "usage:\ngen-all-assimp-parsers.sh include_path src_path"
}

function print_args_error
{
    echo "missing args: 2 needed"
}

if [ ! -z $1 ]; then
    include_path=$1
else
    print_args_error
    print_usage
    exit 1
fi

if [ ! -z $2 ]; then
    src_path=$2
else
    print_args_error
    print_usage
    exit 1
fi

mkdir -p ${include_path} ${src_path}

source assimp-parser-decl.sh

for loader in ${!loaders[@]}; do
    parser_name=${loader}

    parser_info=${loaders[${loader}]}
    parser_info_array=($parser_info)

    class_prefix=${parser_info_array[0]}
    assimp_header_file=${parser_info_array[1]}
    assimp_class=${parser_info_array[2]}

    ./gen-assimp-parser.sh \
        ${parser_name} \
        ${class_prefix}${minko_class_suffix} \
        ${assimp_header_file} \
        ${assimp_class} \
        ${include_path} \
        ${src_path}
done

echo "all files successfully generated"
