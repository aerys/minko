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
	assimp_file_name=${loaders[${loader}]}

	./gen-assimp-parser.sh ${parser_name} ${assimp_file_name} ${include_path} ${src_path}
done

echo "all files successfully generated"
