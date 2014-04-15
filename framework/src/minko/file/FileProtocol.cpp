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

#include "minko/file/FileProtocol.hpp"

#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/async/Worker.hpp"

#include <fstream>
#include <regex>

using namespace minko;
using namespace minko::file;

FileProtocol::FileProtocol()
{
}

void
FileProtocol::load()
{
    auto filename = _file->filename();
    auto options = _options;
	auto flags = std::ios::in | std::ios::ate | std::ios::binary;
	
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
	
	std::fstream file(cleanFilename, flags);

	if (!file.is_open())
		for (auto path : _options->includePaths())
		{
			auto testFilename = options->uriFunction()(File::sanitizeFilename(path + '/' + cleanFilename));

			file.open(testFilename, flags);
			if (file.is_open())
            {
                if (testFilename.find(File::getBinaryDirectory() + "/") != std::string::npos)
                {
                    testFilename = testFilename.substr(File::getBinaryDirectory().size() + 1, testFilename.size());
                }

                realFilename = testFilename;
                break;
            }
		}

    auto loader = shared_from_this();

	if (file.is_open())
	{
        resolvedFilename(realFilename);

		if (_options->loadAsynchronously() && AbstractCanvas::defaultCanvas() != nullptr
            && AbstractCanvas::defaultCanvas()->isWorkerRegistered("file-loader"))
		{
			file.close();
			auto worker = AbstractCanvas::defaultCanvas()->getWorker("file-loader");

			_workerSlots.push_back(worker->message()->connect([=](async::Worker::Ptr, async::Worker::Message message) {
				if (message.type == "complete")
				{
					void* bytes = &*message.data.begin();
					data().assign(static_cast<unsigned char*>(bytes), static_cast<unsigned char*>(bytes) + message.data.size());
					_complete->execute(shared_from_this());					
				}
				else if (message.type == "progress")
				{
					float ratio = *reinterpret_cast<float*>(&*message.data.begin());
					_progress->execute(shared_from_this(), ratio);
				}
				else if (message.type == "error")
				{
					_error->execute(shared_from_this());
				}
			}));

			std::vector<char> input(resolvedFilename().begin(), resolvedFilename().end());

			worker->start(input);
		}
		else
		{
			unsigned int size = (unsigned int)file.tellg();

			// FIXME: use fixed size buffers and call _progress accordingly

            _progress->execute(shared_from_this(), 0.0);

			data().resize(size);

			file.seekg(0, std::ios::beg);
			file.read((char*)&data()[0], size);
			file.close();

            _progress->execute(loader, 1.0);

            _complete->execute(shared_from_this());
		}
	}
	else
        _error->execute(shared_from_this());
}
