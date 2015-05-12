#!/bin/bash

CMFT_BIN=cmft
CONVERT_BIN=convert

INPUT_EXTENSION="${1##*.}"
INPUT_FILENAME="${1%%.$INPUT_EXTENSION}"

OUTPUT_DIR=$INPUT_FILENAME
# OUTPUT_EXTENSION=$INPUT_EXTENSION
OUTPUT_EXTENSION="png"
OUTPUT_LAYOUT=latlong # hcross,vcross,latlong
TMP_INPUT=$(mktemp).tga

# convert to TGA for CMFT

mkdir -p $OUTPUT_DIR
echo "Convert $1 to $TMP_INPUT..."
$CONVERT_BIN $1 $TMP_INPUT

# generate diffuse map

TMP_FILE=$(mktemp)

$CONVERT_BIN $1 $TMP_FILE.tga

$CMFT_BIN                                       \
    --input $TMP_FILE.tga                       \
    --filter none                               \
    --ouputNum 1                                \
    --output0 $TMP_FILE                         \
    --output0params tga,bgra8,$OUTPUT_LAYOUT

$CONVERT_BIN $TMP_FILE.tga -set colorspace RGB $OUTPUT_DIR/$INPUT_FILENAME-diffuse.$OUTPUT_EXTENSION

# generate irradiance map

TMP_DIR=$(mktemp -d)

$CMFT_BIN                                       \
    --input $TMP_INPUT                          \
    --filter irradiance                         \
    --dstFaceSize 256                           \
    --outputNum 1                               \
    --output0 $TMP_DIR/$INPUT_FILENAME          \
    --output0params tga,bgra8,$OUTPUT_LAYOUT

$CONVERT_BIN $TMP_DIR/* -background black +append -set colorspace RGB $OUTPUT_DIR/$INPUT_FILENAME-irradiance.$OUTPUT_EXTENSION

# generate radiance map

TMP_DIR=$(mktemp -d)
TMP_FILE=$(mktemp)

$CONVERT_BIN $1 -resize 2048 -set colorspace RGB $TMP_FILE.tga

$CMFT_BIN                                       \
    --input $TMP_FILE.tga                       \
    --filter radiance                           \
    --srcFaceSize 256                           \
    --edgeFixup warp                            \
    --glossScale 10                             \
    --mipCount 100                              \
    --excludeBase true                          \
    --glossBias 1                               \
    --lightingModel phongbrdf                   \
    --dstFaceSize 256                           \
    --numCpuProcessingThreads 4                 \
    --useOpenCL true                            \
    --clVendor anyGpuVendor                     \
    --deviceType gpu                            \
    --deviceIndex 0                             \
    --inputGammaNumerator 1.0                   \
    --inputGammaDenominator 1.0                 \
    --outputGammaNumerator 1.0                  \
    --outputGammaDenominator 1.0                \
    --outputNum 1                               \
    --output0 $TMP_DIR/$INPUT_FILENAME          \
    --output0params tga,bgra8,$OUTPUT_LAYOUT

if [ $OUTPUT_LAYOUT == "latlong" ]; then
    $CONVERT_BIN $TMP_DIR/${INPUT_FILENAME}_8_4x2.tga -resize 2x1 $TMP_DIR/${INPUT_FILENAME}_9_2x1.tga
    $CONVERT_BIN $TMP_DIR/${INPUT_FILENAME}_8_4x2.tga -resize 1x1 $TMP_DIR/${INPUT_FILENAME}_10_1x1.tga
fi

# $CONVERT_BIN $TMP_FILE.tga $TMP_DIR/* -background black -append $OUTPUT_DIR/$INPUT_FILENAME-radiance.$OUTPUT_EXTENSION
TMP_FILES=`ls $TMP_DIR | sed s/${INPUT_FILENAME}_// | sort -g | sed s#^#$TMP_DIR/${INPUT_FILENAME}_#`

$CONVERT_BIN $TMP_FILES -background black -append -set colorspace RGB $OUTPUT_DIR/$INPUT_FILENAME-radiance.$OUTPUT_EXTENSION
