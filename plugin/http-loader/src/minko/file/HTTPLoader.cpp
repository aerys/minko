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

#include "minko/file/HTTPLoader.hpp"

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

std::list<std::shared_ptr<HTTPLoader>>
HTTPLoader::_runningLoaders;

uint
HTTPLoader::_uid = 0;

HTTPLoader::HTTPLoader()
{
}

void
HTTPLoader::progressHandler(void* arg, int progress)
{
	std::cout << "HTTPLoader::progressHandler(): " << progress << std::endl;
	auto iterator = std::find_if(HTTPLoader::_runningLoaders.begin(),
								 HTTPLoader::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPLoader> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPLoader::_runningLoaders.end())
	{
		std::cerr << "HTTPLoader::progressHandler(): cannot find loader" << std::endl;
		return;
	}
	std::cout << "HTTPLoader::progressHandler(): found loader " << format("%d", progress) << "%"  << std::endl;
	std::shared_ptr<HTTPLoader> loader = *iterator;

	loader->_progress->execute(loader, float(progress) / 100.0f);
}

void
HTTPLoader::completeHandler(void* arg, void* data, int size)
{
	std::cout << "HTTPLoader::completeHandler(): size: " << size << std::endl;
	auto iterator = std::find_if(HTTPLoader::_runningLoaders.begin(),
								 HTTPLoader::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPLoader> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPLoader::_runningLoaders.end())
	{
		std::cerr << "HTTPLoader::completeHandler(): cannot find loader" << std::endl;
		return;
	}

	std::cout << "HTTPLoader::completeHandler(): found loader" << std::endl;
	std::shared_ptr<HTTPLoader> loader = *iterator;

	std::cout << "HTTPLoader::completeHandler(): set data" << std::endl;
	loader->_data.assign(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + size);

	loader->_progress->execute(loader, 1.0);
	std::cout << "HTTPLoader::completeHandler(): call execute" << std::endl;
	loader->_complete->execute(loader);

	std::cout << "HTTPLoader::completeHandler(): remove loader" << std::endl;
	// HTTPLoader::_runningLoaders.remove(loader);
	std::cout << "HTTPLoader::completeHandler(): complete" << std::endl;
}

void
HTTPLoader::errorHandler(void* arg)
{
	std::cout << "HTTPLoader::errorHandler(): " << std::endl;
	auto iterator = std::find_if(HTTPLoader::_runningLoaders.begin(),
								 HTTPLoader::_runningLoaders.end(),
								 [=](std::shared_ptr<HTTPLoader> loader) -> bool {
		return loader.get() == arg;
	});

	if (iterator == HTTPLoader::_runningLoaders.end())
	{
		std::cerr << "HTTPLoader::errorHandler(): cannot find loader" << std::endl;
		return;
	}

	std::cout << "HTTPLoader::errorHandler(): found loader" << std::endl;
	std::shared_ptr<HTTPLoader> loader = *iterator;

	std::cout << "HTTPLoader::errorHandler(): call execute" << std::endl;
	loader->_error->execute(loader);

	std::cout << "HTTPLoader::completeHandler(): remove loader" << std::endl;
	// HTTPLoader::_runningLoaders.remove(loader);
	std::cout << "HTTPLoader::errorHandler(): complete" << std::endl;
}

void
HTTPLoader::load(const std::string& filename, std::shared_ptr<Options> options)
{
	_options = options;

	std::cout << "HTTPLoader::load(): " << filename << std::endl;
	_filename = filename;
	_resolvedFilename = _options->uriFunction()(sanitizeFilename(filename));

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

	_options->loaderFunction([](const std::string& filename, std::shared_ptr<AssetLibrary> assets) -> std::shared_ptr<AbstractLoader>
	{
		return HTTPLoader::create();
	});
	
	auto loader = shared_from_this();

	_runningLoaders.push_back(std::static_pointer_cast<HTTPLoader>(loader));

	loader->progress()->execute(loader, 0.0);

#if defined(EMSCRIPTEN)
	if (options->loadAsynchronously())
	{
		std::cout << "HTTPLoader::load(): " << "call emscripten_async_wget_data " << std::endl;
		emscripten_async_wget_data(_filename.c_str(), loader.get(), &completeHandler, &errorHandler);
		//TODO : use emscripten_async_wget2_data once it has been added
	}
	else
	{
		std::string eval = "";

		eval += "var xhr = new XMLHttpRequest();\n";
		eval += "xhr.open('GET', '" + _filename + "', false);\n";

		eval += "xhr.overrideMimeType('text/plain; charset=x-user-defined');\n";

		eval += "xhr.send(null);\n";

		eval += "if (xhr.status == 200)\n";
		eval += "{\n";
		eval += "	var array = new Uint8Array(xhr.responseText.length);";

		eval +=	"	for(var i = 0; i < xhr.responseText.length; ++i)\n";
		eval +=	"		array[i] = xhr.responseText.charCodeAt(i) & 0xFF;\n";

		eval += "	window.httpLoaderTmpBuffer = Module._malloc(xhr.responseText.length);\n";
		eval += "	Module.HEAPU8.set(array, window.httpLoaderTmpBuffer);\n";

		eval += "	(xhr.responseText.length);\n";
		eval += "}\n";
		eval += "else\n";
		eval += "{\n";
		eval += "	(-1);";
		eval += "}\n";

		int size = emscripten_run_script_int(eval.c_str());

		if (size >= 0)
		{
			eval = "(window.httpLoaderTmpBuffer)";

			unsigned char* bytes = (unsigned char*)emscripten_run_script_int(eval.c_str());

			completeHandler(loader.get(), (void*)bytes, size);
		}
		else
		{
			errorHandler(loader.get());
		}
	}
#else
	/*if (options->loadAsynchronously())
	{*/
		auto worker = AbstractCanvas::defaultCanvas()->getWorker("http");

		_workerSlots.push_back(worker->complete()->connect([=](Worker::MessagePtr data) {
			completeHandler(loader.get(), &*data->begin(), data->size());
		}));

		_workerSlots.push_back(worker->progress()->connect([=](float ratio) {
			progressHandler(loader.get(), ratio * 100);
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
HTTPLoader::wget2CompleteHandler(void* arg, const char* file)
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
