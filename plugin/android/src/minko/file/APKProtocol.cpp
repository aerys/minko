/*
Copyright (c) 2014 Aerys

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

#include "minko/file/APKProtocol.hpp"

#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"

#include "SDL_rwops.h"

using namespace minko;
using namespace minko::file;

APKProtocol::APKProtocol()
{
}

void
APKProtocol::load()
{
	auto filename = _file->filename();
	auto options = _options;

	std::string cleanFilename = "";

	for(uint i = 0; i < filename.length(); ++i)
	{
		if (i < filename.length() - 2 && filename.at(i) == ':' && filename.at(i + 1) == '/' && filename.at(i + 2) == '/')
		{
			cleanFilename = "";
			i += 2;
			continue;
		}

		cleanFilename += filename.at(i);
	}

	_options = options;

	auto realFilename = options->uriFunction()(File::sanitizeFilename(cleanFilename));

	SDL_RWops* file = SDL_RWFromFile(cleanFilename.c_str(), "rb");

	if (!file)
		for (auto path : _options->includePaths())
		{
			auto testFilename = options->uriFunction()(File::sanitizeFilename(path + '/' + cleanFilename));

			file = SDL_RWFromFile(testFilename.c_str(), "rb");
			if (file)
			{
				realFilename = testFilename;
				break;
			}
		}

    auto loader = shared_from_this();

	if (file)
	{
		unsigned int size = file->size(file);

		_progress->execute(shared_from_this(), 0.0);

		data().resize(size);

		file->seek(file, RW_SEEK_SET, 0);
		file->read(file, (char*) &data()[0], size, 1);
		file->close(file);

		_progress->execute(loader, 1.0);

		_complete->execute(shared_from_this());
	}
	else
		_error->execute(shared_from_this());
}
