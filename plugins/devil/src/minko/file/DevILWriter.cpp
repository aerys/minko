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

#include "DevILWriter.hpp"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

using namespace minko::file;

void
DevILWriter::write(const std::string&                 filename,
                 const std::vector<unsigned char>&  data,
                 minko::uint                        width,
                 minko::uint                        height)
{
	ILuint devilID;

	ilInit();
	iluInit();

	ilGenImages(1, &devilID);
	ilBindImage(devilID);

	ilLoadL(IL_TYPE_UNKNOWN, &data[0], data.size());
	checkError();

	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(filename.c_str());
	checkError();

	ilShutDown();
}

void
DevILWriter::checkError()
{
	ILuint error = ilGetError();

	if (error != IL_NO_ERROR)
		throw std::runtime_error(std::string("DevILWriter::write"));
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

