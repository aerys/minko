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

declare -A loaders

loaders["3DS"]="3DSLoader"
loaders["AC"]="ACLoader"
loaders["ASE"]="ASELoader"
loaders["B3D"]="B3DImporter"
loaders["Blender"]="BlenderLoader"
loaders["BVH"]="BVHLoader"
loaders["COB"]="COBLoader"
loaders["Collada"]="ColladaLoader"
loaders["CSM"]="CSMLoader"
loaders["DFX"]="DFXLoader"
loaders["FBX"]="FBXImporter"
loaders["HMP"]="HMPLoader"
loaders["IFC"]="IFCLoader"
loaders["IRR"]="IRRLoader"
loaders["LWO"]="LWOLoader"
loaders["LWS"]="LWSLoader"
loaders["MD2"]="MD2Loader"
loaders["MD3"]="MD3Loader"
loaders["MD5"]="MD5Loader"
loaders["MDC"]="MDCLoader"
loaders["MDL"]="MDLLoader"
loaders["MS3D"]="MS3DLoader"
loaders["NDO"]="NDOLoader"
loaders["NFF"]="NFFLoader"
loaders["ObjFile"]="ObjFileImporter"
loaders["ObjFileMtl"]="ObjFileMtlImporter"
loaders["OFF"]="OFFLoader"
loaders["Ogre"]="OgreImporter"
loaders["Ply"]="PlyLoader"
loaders["Q3BSPFile"]="Q3BSPFileImporter"
loaders["Q3D"]="Q3DLoader"
loaders["Raw"]="RawLoader"
loaders["SMD"]="SMDLoader"
loaders["STL"]="STLLoader"
loaders["Terragen"]="TerragenLoader"
loaders["Unreal"]="UnrealLoader"
loaders["XFile"]="XFileImporter"
loaders["XGL"]="XGLLoader"

for loader in ${!loaders[@]}; do
	parser_name=${loader}
	assimp_file_name=${loaders[${loader}]}
	
	./gen-assimp-parser.sh ${parser_name} ${assimp_file_name} ${include_path} ${src_path}
done

echo "all files successfully generated"
