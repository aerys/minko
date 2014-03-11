if [ -z ${MINKO_HOME} ]; then
	echo "MINKO_HOME variable is empty"
	exit 1
fi

include_path=${MINKO_HOME}/plugin/assimp/include/minko/file
src_path=${MINKO_HOME}/plugin/assimp/src/minko/file

echo "include path: ${include_path}"
echo "src path: ${src_path}"

./gen-all-assimp-parsers ${include_path} ${src_path}

