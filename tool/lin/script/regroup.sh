#! /bin/bash

function array_contains # array value
{
    [[ -n "$1" && -n "$2" ]] || {
        echo "usage: array_contains <array> <value>"
        echo "Returns 0 if array contains value, 1 otherwise"
        return 2
    }

    eval 'local values=("${'$1'[@]}")'

    local element
    for element in "${values[@]}"; do
        [[ "$element" == "$2" ]] && return 0
    done
    return 1
}

availablePlatforms=("windows32" "windows64" "linux32" "linux64" "osx64" "html5" "android" "ios")
availableConfig=("debug" "release")
availableType=("example" "tutorial" "plugin")

if [ "$#" -ne 4 ] || ! array_contains availablePlatforms $1 || ! array_contains availableConfig $2 || ! array_contains availableType $3; then
  echo "Usage: pack.sh \$1 \$2 \$3 \$4" >&2
  echo " \$1: the platform (among: windows32, windows64, linux32, linux64, osx64, html5, android or ios)" >&2
  echo " \$2: the configuration (among: debug or release)" >&2
  echo " \$3: the type of projects you want to pack (among: example, tutorial or plugin)" >&2
  echo " \$4: the output directory where you want to copy all files" >&2
  exit 1
fi

platform=$1
config=$2
type=$3
outputDir=$4

completeOutputDir=$outputDir/$platform/$config/$type

mkdir -p $completeOutputDir

for dir in $type/*; do

    dir_name=${dir##*/}
	sourceDir=$type/${dir_name}/bin/$platform/$config
    
	if [ -d $sourceDir ]; then
		echo ${dir_name}
		
		mkdir -p $completeOutputDir/${dir_name}
		
		cp -rf $sourceDir/* $completeOutputDir/${dir_name}
	fi
done

exit 0