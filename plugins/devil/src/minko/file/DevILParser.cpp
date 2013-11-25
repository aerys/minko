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
	ILuint error;

	int width = -1;
	int height = -1;
	int format = -1;
	
	// DevIL reference : http://www-f9.ijs.si/~matevz/docs/DevIL/apireference.html

	ilInit();
	iluInit();

	ilSetInteger(IL_FORMAT_MODE, IL_RGBA);

	ilGenImages(1, &devilID);
	ilBindImage(devilID);

	ilLoadL(IL_TYPE_UNKNOWN, &data[0], data.size());

	error = ilGetError();

	if (error == IL_NO_ERROR)
	{
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);

		format = ilGetInteger(IL_IMAGE_FORMAT);


		if (format == IL_BGR || format == IL_BGRA)
			iluSwapColours();

		format = ilGetInteger(IL_IMAGE_FORMAT);

		auto bmpData = ilGetData();

		auto texture = render::Texture::create(options->context(), width, height, options->generateMipmaps());

		texture->data(bmpData, format == IL_RGBA ? minko::render::Texture::RGBA : minko::render::Texture::RGB);
		texture->upload();

		AssetLibrary->texture(filename, texture);
		
		complete()->execute(shared_from_this());
	}
	else
	{
		throw("Error");
	}
	ilShutDown();
}
