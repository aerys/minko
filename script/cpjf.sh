#!/bin/bash

# The goal of this script is to copy all Java files from
# an absolute source folder to a relative destination directory
# keeping the hierarchy of each file

if [ "$#" -ne 2 ]; then
    echo "usage: copy_java.sh absoluteSrcDir relativeDestDir" 2> /dev/null
    exit 1
fi

if [ $OSTYPE == "cygwin" ]; then
	curDir=$(cygpath -w $(pwd))
else
	curDir=$(pwd)
fi

#echo "Current directory: $curDir"

if [ -d $1 ]; then
	# Move to the current plugin directory
	pushd $1 > /dev/null

	java_files=$(find . -iname '*.java')

	for file in $java_files 
	do
		dir="$2/"$(dirname "$file")
		# create the dest dir if doesn't exist
		if [ ! -d "$curDir/$dir" ]; then
			#echo "Make a new directory: $dir"
			mkdir -p "$curDir/$dir"
		fi

		# copy Java file
		#echo "Copy file $1/$file to $curDir/$dir"
		cp "$1/$file" "$curDir/$dir"
	done

	popd > /dev/null
else
	echo "warning: $1 not found."
fi