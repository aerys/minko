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

#include "minko/file/FileLoader.hpp"

#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"

#include <fstream>
#include <regex>

using namespace minko;
using namespace minko::file;

FileLoader::FileLoader()
{
}

void
FileLoader::load(const std::string& filename, std::shared_ptr<Options> options)
{
	auto flags = std::ios::in | std::ios::ate | std::ios::binary;
	
	std::string cleanFilename = "";

	for(int i = 0; i < filename.length(); ++i)
	{
		if (i < filename.length() - 2 && filename.at(i) == ':' && filename.at(i + 1) == '/' && filename.at(i + 2) == '/')
		{
			cleanFilename = "";
			i += 2;
			continue;
		}
		
		cleanFilename += filename.at(i);
	}

	_filename = filename;
	_resolvedFilename = options->uriFunction()(sanitizeFilename(cleanFilename));
	_options = options;
	
	std::fstream file(cleanFilename, flags);

	if (!file.is_open())
		for (auto path : _options->includePaths())
		{
			auto testFilename = options->uriFunction()(sanitizeFilename(path + '/' + cleanFilename));

			file.open(testFilename, flags);
			if (file.is_open())
            {
				_resolvedFilename = testFilename;
				break;
            }
		}

	if (file.is_open())
	{
		unsigned int size = (unsigned int)file.tellg();

		// FIXME: use fixed size buffers and call _progress accordingly

		_progress->execute(shared_from_this());

		_data.resize(size);

		file.seekg(0, std::ios::beg);
		file.read((char*)&_data[0], size);
		file.close();

		_progress->execute(shared_from_this());
	
		_complete->execute(shared_from_this());
	}
	else
		_error->execute(shared_from_this());
}
