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

#include "DevILParser.hpp"

#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Texture.hpp"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

using namespace minko::file;

void
DevILParser::parse(const std::string&                 filename,
                 const std::string&                 resolvedFilename,
                 std::shared_ptr<Options>           options,
                 const std::vector<unsigned char>&  data,
                 std::shared_ptr<AssetLibrary>      AssetLibrary)
{
	ILuint devilID;

	int width = -1;
	int height = -1;
	int format = -1;
	
	// DevIL reference : http://www-f9.ijs.si/~matevz/docs/DevIL/apireference.html

	ilInit();
	iluInit();

	ilGenImages(1, &devilID);
	ilBindImage(devilID);

	ilLoadL(IL_TYPE_UNKNOWN, &data[0], data.size());

	checkError();

	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	format = ilGetInteger(IL_IMAGE_FORMAT);


	if (format == IL_BGR || format == IL_BGRA)
		iluSwapColours();

	checkError();

	format = ilGetInteger(IL_IMAGE_FORMAT);

	auto bmpData = ilGetData();

	checkError();

	auto texture = render::Texture::create(options->context(), width, height, options->generateMipmaps());

	texture->data(bmpData, format == IL_RGBA ? minko::render::Texture::RGBA : minko::render::Texture::RGB);
	texture->upload();

	AssetLibrary->texture(filename, texture);
		
	complete()->execute(shared_from_this());

	ilShutDown();
}


void
DevILParser::checkError()
{
	ILuint error = ilGetError();

	if (error != IL_NO_ERROR)
		throw std::runtime_error(std::string("DevILParser::parse"));
}

std::set<std::string>
DevILParser::getSupportedFileExensions()
{
	std::set<std::string> result;

	result.insert("bmp");
	result.insert("cut");
	result.insert("dcx");
	result.insert("dicom");
	result.insert("dcm");
	result.insert("dds");
	//result.insert("exr");
	result.insert("fits");
	result.insert("fit");
	result.insert("ftx");
	result.insert("hdr");
	result.insert("icns");
	result.insert("ico");
	result.insert("cur");
	result.insert("iff");
	result.insert("iwi");
	result.insert("gif");
	result.insert("jpg");
	result.insert("jpe");
	result.insert("jpeg");
	result.insert("jp2");
	result.insert("lbm");
	result.insert("lif");
	result.insert("mdl");
	result.insert("mp3");
	result.insert("pal");
	result.insert("pcd");
	result.insert("pcx");
	result.insert("pic");
	result.insert("png");
	result.insert("pbm");
	result.insert("pgm");
	result.insert("pnm");
	result.insert("pix");
	result.insert("psd");
	result.insert("psp");
	result.insert("pxr");
	result.insert("raw");
	result.insert("rot");
	result.insert("sgi");
	result.insert("bw");
	result.insert("rgb");
	result.insert("rgba");
	result.insert("texture");
	result.insert("tga");
	result.insert("tif");
	result.insert("tpl");
	result.insert("utx");
	result.insert("wal");
	result.insert("vtf");
	//result.insert("wdp");
	result.insert("hdp");
	result.insert("xpm");

	return result;
}
