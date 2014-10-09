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
HTTPProtocol::progressHandler(void* arg, int loadedBytes, int totalBytes)
{
    int progress = 0;

    if (totalBytes != 0)
        progress = (int)(100.f * ((float)loadedBytes / (float)totalBytes));

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

    loader->_progress->execute(loader, float(progress));
}

void
HTTPProtocol::completeHandler(void* arg, void* data, unsigned int size)
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
HTTPProtocol::errorHandler(void* arg, int code, const char * message)
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

    resolvedFilename(_file->filename());

    std::cout << resolvedFilename() << std::endl;

    if (_options->includePaths().size() != 0)
    {
        if (resolvedFilename().substr(0, 7) != "http://" && resolvedFilename().substr(0, 8) != "https://")
        {
            for (auto path : _options->includePaths())
            {
                resolvedFilename(path + '/' + resolvedFilename());
                break;
            }
        }
    }
    std::cout << resolvedFilename() << std::endl;

    _options->protocolFunction([](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
    {
        return HTTPProtocol::create();
    });

    auto loader = shared_from_this();

    _runningLoaders.push_back(std::static_pointer_cast<HTTPProtocol>(loader));

    loader->progress()->execute(loader, 0.0);

#if defined(EMSCRIPTEN)
    if (options()->loadAsynchronously())
    {
        std::cout << "HTTPProtocol::load(): " << "call emscripten_async_wget_data " << std::endl;

        //EMSCRIPTEN < 1.13.1
        //emscripten_async_wget_data(resolvedFilename().c_str(), loader.get(), &completeHandler, &errorHandler);

        //EMSCRIPTEN >= 1.13.1
        emscripten_async_wget2_data(
            resolvedFilename().c_str(),
            "GET",
            "",
            loader.get(),
            0,
            &wget2CompleteHandler,
            &wget2ErrorHandler,
            &wget2ProgressHandler
        );
    }
    else
    {
        std::string eval = "";

        eval += "var xhr = new XMLHttpRequest();\n";
        eval += "xhr.open('GET', '" + resolvedFilename() + "', false);\n";

        eval += "xhr.overrideMimeType('text/plain; charset=x-user-defined');\n";

        eval += "xhr.send(null);\n";

        eval += "if (xhr.status == 200)\n";
        eval += "{\n";
        eval += "    var array = new Uint8Array(xhr.responseText.length);";

        eval +=    "    for(var i = 0; i < xhr.responseText.length; ++i)\n";
        eval +=    "        array[i] = xhr.responseText.charCodeAt(i) & 0xFF;\n";

        eval += "    window.HTTPProtocolTmpBuffer = Module._malloc(xhr.responseText.length);\n";
        eval += "    Module.HEAPU8.set(array, window.HTTPProtocolTmpBuffer);\n";

        eval += "    (xhr.responseText.length);\n";
        eval += "}\n";
        eval += "else\n";
        eval += "{\n";
        eval += "    (-1);";
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
                progressHandler(loader.get(), int(ratio * 100.f), 100);
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
            progressHandler(loader.get(), int(p * 100.f), 100);
        });

        request.run();

        std::vector<char>& output = request.output();

        completeHandler(loader.get(), &*output.begin(), output.size());
    }
#endif
}

#if defined(EMSCRIPTEN)
void
HTTPProtocol::wget2CompleteHandler(unsigned int id, void* arg, void* data, unsigned int size)
{
    HTTPProtocol::completeHandler(arg, data, size);
}

void
HTTPProtocol::wget2ErrorHandler(unsigned int id, void* arg, int code, const char* message)
{
    HTTPProtocol::errorHandler(arg, code, message);
}

void
HTTPProtocol::wget2ProgressHandler(unsigned int id, void* arg, int loadedBytes, int totalBytes)
{
    HTTPProtocol::progressHandler(arg, loadedBytes, totalBytes);
}
#endif
