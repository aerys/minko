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

#include "minko/net/HTTPProtocol.hpp"

#include "minko/async/Worker.hpp"
#include "minko/net/HTTPRequest.hpp"
#include "minko/file/Options.hpp"
#include "minko/Signal.hpp"
#include "minko/AbstractCanvas.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::async;
using namespace minko::net;

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
	loader->data().assign(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + size);

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
	std::cout << "HTTPProtocol::load(): " << _file->filename() << std::endl;

	if (_options->includePaths().size() != 0)
	{
		if (resolvedFilename().find_first_of("http://") != 0 && resolvedFilename().find_first_of("https://") != 0)
		{
			for (auto path : _options->includePaths())
			{
				resolvedFilename(path + '/' + resolvedFilename());
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
	if (options->loadAsynchronously())
	{
		std::cout << "HTTPProtocol::load(): " << "call emscripten_async_wget_data " << std::endl;
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

		eval += "	window.HTTPProtocolTmpBuffer = Module._malloc(xhr.responseText.length);\n";
		eval += "	Module.HEAPU8.set(array, window.HTTPProtocolTmpBuffer);\n";

		eval += "	(xhr.responseText.length);\n";
		eval += "}\n";
		eval += "else\n";
		eval += "{\n";
		eval += "	(-1);";
		eval += "}\n";

		int size = emscripten_run_script_int(eval.c_str());

		if (size >= 0)
		{
			eval = "(window.HTTPProtocolTmpBuffer)";

			unsigned char* bytes = (unsigned char*)emscripten_run_script_int(eval.c_str());

			completeHandler(loader.get(), (void*)bytes, size);
		}
		else
		{
			errorHandler(loader.get());
		}
	}
#else
	if (options()->loadAsynchronously())
	{
		auto worker = AbstractCanvas::defaultCanvas()->getWorker("http");

		_workerSlots.push_back(worker->message()->connect([=](Worker::Ptr, Worker::Message message) {
			if (message.type == "complete")
			{
				completeHandler(loader.get(), &*message.data.begin(), message.data.size());
			}
			else if (message.type == "progress")
			{
				float ratio = *reinterpret_cast<float*>(&*message.data.begin());
				progressHandler(loader.get(), int(ratio * 100.f));				
			}
			else if (message.type == "error")
			{
				errorHandler(loader.get());
			}
		}));

		std::vector<char> input(resolvedFilename().begin(), resolvedFilename().end());
		worker->start(input);
	}
	else
	{
		HTTPRequest request(resolvedFilename());

		request.progress()->connect([&](float p){
			progressHandler(loader.get(), int(p * 100.f));
		});

		request.run();

		std::vector<char>& output = request.output();

		completeHandler(loader.get(), &*output.begin(), output.size());
	}
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
