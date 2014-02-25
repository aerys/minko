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
	// DevIL reference : http://www-f9.ijs.si/~matevz/docs/DevIL/apireference.html

	ILuint devilID;
	
	ilInit();
	iluInit();

	ilGenImages(1, &devilID);
	ilBindImage(devilID);

	ilLoadL(IL_TYPE_UNKNOWN, &data[0], data.size());
	checkError();
	
	int format = ilGetInteger(IL_IMAGE_FORMAT);

	if (format == IL_BGR || format == IL_BGRA)
	{
		iluSwapColours();
		checkError();
		format = ilGetInteger(IL_IMAGE_FORMAT);
	}

	if (ilGetInteger(IL_IMAGE_ORIGIN) == IL_ORIGIN_LOWER_LEFT)
	{
		iluFlipImage();
		checkError();
	}

	computeDimensions();

	auto bmpData = ilGetData();
	checkError();

	auto texture = render::Texture::create(options->context(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), options->generateMipmaps());

	texture->data(bmpData, format == IL_RGBA ? minko::render::TextureFormat::RGBA : minko::render::TextureFormat::RGB);
	texture->upload();

	AssetLibrary->texture(filename, texture);
		
	complete()->execute(shared_from_this());

	ilShutDown();
}

void
DevILParser::computeDimensions()
{
	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);

	int wPow = std::ceil(std::log(double(width)) / std::log(double(2u)));
	int hPow = std::ceil(std::log(double(height)) / std::log(double(2u)));

	int w = std::pow(2.f, wPow);
	int h = std::pow(2.f, hPow);

	if (w > width)
		wPow--;

	if (h > height)
		hPow--;

	int maxPow = std::max(wPow, hPow);

	if (maxPow < 1)
		maxPow = 1;

	w = std::pow(2.f, maxPow);

	if (w > render::AbstractTexture::MAX_SIZE)
		w = render::AbstractTexture::MAX_SIZE;

	h = w;

	if (w != width || h != height)
	{
		iluImageParameter(ILU_FILTER, ILU_BILINEAR);
		iluScale(w, h, ilGetInteger(IL_IMAGE_DEPTH));
		checkError();
	}
}


void
DevILParser::checkError()
{
	ILuint error = ilGetError();

	if (error != IL_NO_ERROR)
		throw std::runtime_error(std::string("DevILParser::parse"));
}

std::vector<std::string>
DevILParser::getSupportedFileExensions()
{
	std::vector<std::string> result;

	result.push_back("bmp");
	result.push_back("cut");
	result.push_back("dcx");
	result.push_back("dicom");
	result.push_back("dcm");
	result.push_back("dds");
	result.push_back("fits");
	result.push_back("fit");
	result.push_back("ftx");
	result.push_back("hdr");
	result.push_back("icns");
	result.push_back("ico");
	result.push_back("cur");
	result.push_back("iff");
	result.push_back("iwi");
	result.push_back("gif");
	result.push_back("jpg");
	result.push_back("jpe");
	result.push_back("jpeg");
	result.push_back("jp2");
	result.push_back("lbm");
	result.push_back("lif");
	result.push_back("mdl");
	result.push_back("mp3");
	result.push_back("pal");
	result.push_back("pcd");
	result.push_back("pcx");
	result.push_back("pic");
	result.push_back("png");
	result.push_back("pbm");
	result.push_back("pgm");
	result.push_back("pnm");
	result.push_back("pix");
	result.push_back("psd");
	result.push_back("psp");
	result.push_back("pxr");
	result.push_back("raw");
	result.push_back("rot");
	result.push_back("sgi");
	result.push_back("bw");
	result.push_back("rgb");
	result.push_back("rgba");
	result.push_back("texture");
	result.push_back("tga");
	result.push_back("tif");
	result.push_back("tpl");
	result.push_back("utx");
	result.push_back("wal");
	result.push_back("vtf");
	result.push_back("hdp");
	result.push_back("xpm");

	return result;
}
