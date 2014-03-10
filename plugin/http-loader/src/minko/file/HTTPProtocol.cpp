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

#include "minko/file/HTTPProtocol.hpp"

#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"
#include "minko/AbstractCanvas.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#else
# include "minko/async/HTTPWorker.hpp"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::async;

std::list<std::shared_ptr<HTTPProtocol>>
HTTPProtocol::_runningLoaders;

uint
HTTPProtocol::_uid = 0;

HTTPProtocol::HTTPProtocol()
{
}

void
HTTPProtocol::progressHandler(void* arg, int progress)
{
	std::cout << "HTTPProtocol::progressHandler(): " << progress << std::endl;
	auto iterator = std::find_if(HTTPProtocol::_runningLoaders.begin(),
								 HTTPProtocol::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPProtocol> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPProtocol::_runningLoaders.end())
	{
		std::cerr << "HTTPProtocol::progressHandler(): cannot find loader" << std::endl;
		return;
	}
	std::cout << "HTTPProtocol::progressHandler(): found loader " << format("%d", progress) << "%"  << std::endl;
	std::shared_ptr<HTTPProtocol> loader = *iterator;

	loader->_progress->execute(loader, float(progress) / 100.0f);
}

void
HTTPProtocol::completeHandler(void* arg, void* data, int size)
{
	std::cout << "HTTPProtocol::completeHandler(): size: " << size << std::endl;
	auto iterator = std::find_if(HTTPProtocol::_runningLoaders.begin(),
								 HTTPProtocol::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPProtocol> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPProtocol::_runningLoaders.end())
	{
		std::cerr << "HTTPProtocol::completeHandler(): cannot find loader" << std::endl;
		return;
	}

	std::cout << "HTTPProtocol::completeHandler(): found loader" << std::endl;
	std::shared_ptr<HTTPProtocol> loader = *iterator;

	std::cout << "HTTPProtocol::completeHandler(): set data" << std::endl;
	loader->_data.assign(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + size);

	loader->_progress->execute(loader, 1.0);
	std::cout << "HTTPProtocol::completeHandler(): call execute" << std::endl;
	loader->_complete->execute(loader);

	std::cout << "HTTPProtocol::completeHandler(): remove loader" << std::endl;
	// HTTPProtocol::_runningLoaders.remove(loader);
	std::cout << "HTTPProtocol::completeHandler(): complete" << std::endl;
}

void
HTTPProtocol::errorHandler(void* arg)
{
	std::cout << "HTTPProtocol::errorHandler(): " << std::endl;
	auto iterator = std::find_if(HTTPProtocol::_runningLoaders.begin(),
								 HTTPProtocol::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPProtocol> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPProtocol::_runningLoaders.end())
	{
		std::cerr << "HTTPProtocol::errorHandler(): cannot find loader" << std::endl;
		return;
	}

	std::cout << "HTTPProtocol::errorHandler(): found loader" << std::endl;
	std::shared_ptr<HTTPProtocol> loader = *iterator;

	std::cout << "HTTPProtocol::errorHandler(): call execute" << std::endl;
	loader->_error->execute(loader);

	std::cout << "HTTPProtocol::completeHandler(): remove loader" << std::endl;
	// HTTPProtocol::_runningLoaders.remove(loader);
	std::cout << "HTTPProtocol::errorHandler(): complete" << std::endl;
}

void
HTTPProtocol::load()
{
	std::cout << "HTTPProtocol::load(): " << _filename << std::endl;
	_resolvedFilename = _options->uriFunction()(sanitizeFilename(_filename));

	if (_options->includePaths().size() != 0)
	{
		if (_resolvedFilename.find_first_of("http://") != 0 && _resolvedFilename.find_first_of("https://") != 0)
		{
			for (auto path : _options->includePaths())
			{
				_resolvedFilename = path + '/' + _resolvedFilename;
				break;
			}
		}
	}

	_options->protocolFunction([](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
	{
		return HTTPProtocol::create();
	});
	
	auto loader = shared_from_this();

	_runningLoaders.push_back(std::static_pointer_cast<HTTPProtocol>(loader));

	loader->progress()->execute(loader, 0.0);

#if defined(EMSCRIPTEN)
	//std::string destFilename = format("prepare%d", _uid++);
	//std::cout << "HTTPProtocol::load(): " << "call emscripten_async_wget2 with filename " << destFilename << std::endl;
	//emscripten_async_wget2(_filename.c_str(), destFilename.c_str(), "GET", "", loader.get(), &wget2CompleteHandler, &errorHandler, &progressHandler);

	std::cout << "HTTPProtocol::load(): " << "call emscripten_async_wget_data " << std::endl;
	emscripten_async_wget_data(_filename.c_str(), loader.get(), &completeHandler, &errorHandler);
#else
	/*if (options->loadAsynchronously())
	{*/
		auto worker = AbstractCanvas::defaultCanvas()->getWorker("http");

		_workerSlots.push_back(worker->complete()->connect([=](Worker::MessagePtr data) {
			completeHandler(loader.get(), &*data->begin(), data->size());
		}));

		_workerSlots.push_back(worker->progress()->connect([=](float ratio) {
			progressHandler(loader.get(), (int)ratio * 100);
		}));

		worker->input(std::make_shared<std::vector<char>>(_resolvedFilename.begin(), _resolvedFilename.end()));
	/*}
	else
	{
		//fixme: handle synchronous HTTP loading
	}*/
#endif
}

#if defined(EMSCRIPTEN)
void
HTTPProtocol::wget2CompleteHandler(void* arg, const char* file)
{
	FILE* f = fopen(file, "rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);

		fseek (f, 0, SEEK_SET);
		char* data = new char[size];
		fread(data, size, 1, f);
		fclose(f);

		completeHandler(arg, (void*)data, size);
		delete data;
	}
}
#endif
