/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/file/DevILWriter.hpp"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

using namespace minko;
using namespace minko::file;

uint
DevILWriter::createScaledImage(const std::vector<unsigned char>&    src,
                               uint                                 srcWidth,
                               uint                                 srcHeight,
                               uint                                 dstWidth,
                               uint                                 dstHeight,
                               uint                                 componentCount)
{
	ILuint devilID;

	ilInit();
	iluInit();

	ilGenImages(1, &devilID);
	ilBindImage(devilID);

    int format;

    switch (componentCount) {
        case 1:  format = IL_LUMINANCE; break;
        case 3:  format = IL_RGB; break;
        case 4:  format = IL_RGBA; break;
        default: return 0;
    }

    ilTexImage(srcWidth,
               srcHeight,
               1,
               componentCount,
               format,
               IL_UNSIGNED_BYTE,
               const_cast<unsigned char*> (src.data()));
    iluFlipImage();
    iluScale(dstWidth, dstHeight, 1);
	checkError();

    return devilID;
}

void
DevILWriter::writeToFile(const std::string&                     filename,
                         const std::vector<unsigned char>&      src,
                         uint                                   srcWidth,
                         uint                                   srcHeight,
                         uint                                   dstWidth,
                         uint                                   dstHeight,
                         uint                                   componentCount)
{
	uint devilID = createScaledImage(src, srcWidth, srcHeight, dstWidth, dstHeight, componentCount);

	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(filename.c_str());
	checkError();

    ilDeleteImages(1, &devilID);

	ilShutDown();
}

void
DevILWriter::writeToStream(std::vector<unsigned char>&          dst,
                           const std::vector<unsigned char>&    src,
                           uint                                 srcWidth,
                           uint                                 srcHeight,
                           uint                                 dstWidth,
                           uint                                 dstHeight,
                           uint                                 componentCount)
{
// TODO
// add enum into minko framework
    static const ILuint imageType = IL_PNG;

	uint devilID = createScaledImage(src, srcWidth, srcHeight, dstWidth, dstHeight, componentCount);

    uint size = ilSaveL(imageType, nullptr, 0);

    unsigned char* buffer = new unsigned char[size];
    ilSaveL(imageType, buffer, size);

    std::copy(buffer, buffer + size, std::back_inserter(dst));

    delete[] buffer;

    ilDeleteImages(1, &devilID);

	ilShutDown();
}

void
DevILWriter::checkError()
{
	ILuint error = ilGetError();

	if (error != IL_NO_ERROR)
		throw std::runtime_error(std::string("DevILWriter::write, ") + iluErrorString(error));
}

std::set<std::string>
DevILWriter::getSupportedFileExensions()
{
	std::set<std::string> result;

	result.insert("bmp");
	result.insert("dds");
	//result.insert("exr");
	result.insert("h");
	result.insert("hdr");
	result.insert("jpg");
	result.insert("jpe");
	result.insert("jpeg");
	result.insert("jp2");
	result.insert("pal");
	result.insert("pcx");
	result.insert("png");
	result.insert("pbm");
	result.insert("pgm");
	result.insert("pnm");
	result.insert("psd");
	result.insert("raw");
	result.insert("sgi");
	result.insert("bw");
	result.insert("rgb");
	result.insert("rgba");
	result.insert("tga");
	result.insert("tif");
	result.insert("vtf");

	return result;
}

