#!/bin/sh
# The script optionally takes two parameters:
# sh ./generate-images.sh [<in_image> [<out_directory>]]
# Example:
# sh ./generate-images.sh ../test_image.jpg images/test

FUNCTIONS=" iluAlienify() iluBlurAvg(10) iluBlurGaussian(10) iluContrast(0.4) iluContrast(1.7) iluEqualize() iluNegative() iluNoisify(0.1) iluNoisify(0.8) iluPixelize(5) iluSharpen(1.8,2) iluSharpen(2.1,3) iluEmboss() iluMirror() iluSaturate1f(0.6) iluWave(1.2) iluGammaCorrect(0.7) iluGammaCorrect(1.6)"
#iluCrop, iluEdgeDetectE, iluEdgeDetectP, iluEdgeDetectS, iluEnlargeCanvas, iluEnlargeImage, iluFlipImage, iluInvertAlpha, iluReplaceColour, iluRotate, iluRotate3D, iluSaturate4f, iluScale, iluScaleAlpha, iluScaleColours
IN_IMAGE=small_stairway.png
test -n "$1" && IN_IMAGE=$1
test -n "$2" && OUT_DIR=$2
#just the filename without the path
IN_IMAGE_NAME=$(echo $IN_IMAGE | sed -e 's/\/*\([^\/]*\/\)*\(.*\)\..*/\2/')
#echo image_name: $IN_IMAGE_NAME
#just the extension
IN_IMAGE_EXT=$(echo $IN_IMAGE | sed -e 's/.*\.\(.*\)/\1/')
#echo image_ext: $IN_IMAGE_EXT
for function in $FUNCTIONS
do
	# Substituing the comma from the parameters list to underscore
	# Substituing the first parenthesis to underscore and throwing away the closing parenthesis
	# Reformatting the output filename
	# and append the same extension as that of the original
	OUT_FILENAME=$OUT_DIR/ilu_${IN_IMAGE_NAME}_$(\
	echo $function | sed -e "s/,/_/g" \
	| sed -e "s/(\([^)]\+\))/_\1/g" \
	| sed -e "s/ilu\(\w\)\(\w*\)(*\([^)]*\))*/\L\1\E\2\3/g")".${IN_IMAGE_EXT}"
	# don't overwrite anything, that doesn't make sense
#	echo out_filename: $OUT_FILENAME
	test -f $OUT_FILENAME && continue
	../bin/ilur -l $IN_IMAGE -s $OUT_FILENAME  -a "$function"
done
