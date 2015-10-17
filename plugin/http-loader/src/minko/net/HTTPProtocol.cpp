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

#include "minko/Signal.hpp"
#include "minko/async/Worker.hpp"
#include "minko/file/Options.hpp"
#include "minko/net/HTTPOptions.hpp"
#include "minko/net/HTTPProtocol.hpp"
#include "minko/net/HTTPRequest.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/log/Logger.hpp"

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
    float progress = 0.0;

    if (totalBytes != 0)
        progress = (float)(loadedBytes) / (float)(totalBytes);

    auto iterator = std::find_if(HTTPProtocol::_runningLoaders.begin(),
                                 HTTPProtocol::_runningLoaders.end(),
                                 [=](std::shared_ptr<HTTPProtocol> loader) -> bool {
        return loader.get() == arg;
    });

    if (iterator == HTTPProtocol::_runningLoaders.end())
    {
        LOG_ERROR("cannot find loader");
        return;
    }

    std::shared_ptr<HTTPProtocol> loader = *iterator;

    loader->_progress->execute(loader, progress);
}

void
HTTPProtocol::completeHandler(void* arg, void* data, unsigned int size)
{
    auto iterator = std::find_if(HTTPProtocol::_runningLoaders.begin(),
                                 HTTPProtocol::_runningLoaders.end(),
                                 [=](std::shared_ptr<HTTPProtocol> loader) -> bool {
        return loader.get() == arg;
    });

    if (iterator == HTTPProtocol::_runningLoaders.end())
    {
        LOG_ERROR("cannot find loader");
        return;
    }

    std::shared_ptr<HTTPProtocol> loader = *iterator;

    loader->data().assign(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + size);

    loader->_progress->execute(loader, 1.0);
    loader->_complete->execute(loader);

    HTTPProtocol::_runningLoaders.remove(loader);
}

void
HTTPProtocol::errorHandler(void* arg, int code, const char * message)
{
    auto iterator = std::find_if(HTTPProtocol::_runningLoaders.begin(),
                                 HTTPProtocol::_runningLoaders.end(),
                                 [=](std::shared_ptr<HTTPProtocol> loader) -> bool {
        return loader.get() == arg;
    });

    if (iterator == HTTPProtocol::_runningLoaders.end())
    {
        LOG_ERROR("cannot find loader");
        return;
    }

    std::shared_ptr<HTTPProtocol> loader = *iterator;

    LOG_ERROR(message);

    loader->_error->execute(loader);

    HTTPProtocol::_runningLoaders.remove(loader);
}

void
HTTPProtocol::load()
{
    _options->protocolFunction([](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
    {
        return HTTPProtocol::create();
    });

    auto loader = shared_from_this();

    _runningLoaders.push_back(std::static_pointer_cast<HTTPProtocol>(loader));

    loader->progress()->execute(loader, 0.0);

    auto username = std::string();
    auto password = std::string();
    auto additionalHeaders = std::unordered_map<std::string, std::string>();
    auto verifyPeer = true;

    auto httpOptions = std::dynamic_pointer_cast<HTTPOptions>(_options);

    if (httpOptions != nullptr)
    {
        username = httpOptions->username();
        password = httpOptions->password();

        additionalHeaders = httpOptions->additionalHeaders();

        verifyPeer = httpOptions->verifyPeer();
    }

    auto seekingOffset = _options->seekingOffset();
    auto seekedLength = _options->seekedLength();

    if (seekingOffset >= 0 && seekedLength > 0)
    {
        auto rangeMin = std::to_string(seekingOffset);
        auto rangeMax = std::to_string(seekingOffset + seekedLength - 1);

        additionalHeaders.insert(std::make_pair(
            "Range",
            "bytes=" + rangeMin + "-" + rangeMax
        ));
    }

#if defined(EMSCRIPTEN)
    if (options()->loadAsynchronously())
    {
        //EMSCRIPTEN < 1.13.1
        //emscripten_async_wget_data(resolvedFilename().c_str(), loader.get(), &completeHandler, &errorHandler);

        //EMSCRIPTEN >= 1.13.1
        auto additionalHeadersJsonString = std::string();

        if (!additionalHeaders.empty())
        {
            auto additionalHeaderCount = 0u;

            additionalHeadersJsonString += "{ ";

            for (const auto& additionalHeader : additionalHeaders)
            {
                additionalHeadersJsonString += std::string("\"" + additionalHeader.first + "\" : \"" + additionalHeader.second + "\"");

                if (additionalHeaderCount < additionalHeaders.size() - 1)
                    additionalHeadersJsonString += ", ";

                ++additionalHeaderCount;
            }

            additionalHeadersJsonString += " }";
        }

        emscripten_async_wget2_data(
            resolvedFilename().c_str(),
            "GET",
            "",
#if defined(EMSCRIPTEN_WGET_HEADERS)
            additionalHeadersJsonString.c_str(),
#endif
            loader.get(),
            true,
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

        for (const auto& additionalHeader : additionalHeaders)
        {
            eval += "xhr.setRequestHeader('" + additionalHeader.first + "', '" + additionalHeader.second + "');\n";
        }

        eval += "xhr.send(null);\n";

        eval += "if (xhr.status == 200 || xhr.status == 206)\n";
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

        const auto offset = _options->seekingOffset();
        const auto length = _options->seekedLength();

        std::stringstream inputStream;

        const auto& resolvedFilename = this->resolvedFilename();
        const int resolvedFilenameSize = static_cast<int>(resolvedFilename.size());

        const int usernameSize = username.size();
        const int passwordSize = password.size();

        const int numAdditionalHeaders = additionalHeaders.size();

        inputStream.write(reinterpret_cast<const char*>(&resolvedFilenameSize), 4);
        if (resolvedFilenameSize > 0)
            inputStream.write(resolvedFilename.data(), resolvedFilenameSize);

        inputStream.write(reinterpret_cast<const char*>(&usernameSize), 4);
        if (usernameSize > 0)
            inputStream.write(username.data(), usernameSize);

        inputStream.write(reinterpret_cast<const char*>(&passwordSize), 4);
        if (passwordSize > 0)
            inputStream.write(password.data(), passwordSize);

        inputStream.write(reinterpret_cast<const char*>(&numAdditionalHeaders), 4);

        for (const auto& additionalHeader : additionalHeaders)
        {
            const auto& key = additionalHeader.first;
            const auto& value = additionalHeader.second;

            const int keySize = static_cast<int>(key.size());
            const int valueSize = static_cast<int>(value.size());

            inputStream.write(reinterpret_cast<const char*>(&keySize), 4);
            if (keySize > 0)
                inputStream.write(reinterpret_cast<const char*>(&valueSize), 4);

            inputStream.write(key.data(), keySize);
            if (valueSize > 0)
                inputStream.write(value.data(), valueSize);
        }

        inputStream.write(reinterpret_cast<const char*>(&verifyPeer), 1);

        auto inputString = inputStream.str();

        worker->start(std::vector<char>(inputString.begin(), inputString.end()));
    }
    else
    {
        HTTPRequest request(resolvedFilename(), username, password, &additionalHeaders);

        request.verifyPeer(verifyPeer);

        auto requestIsSuccessfull = true;

        auto requestErrorSlot = request.error()->connect([&](int error, const std::string& errorMessage)
        {
            requestIsSuccessfull = false;

            this->error()->execute(shared_from_this());
        });

        auto requestProgressSlot = request.progress()->connect([&](float p){
            progressHandler(loader.get(), int(p * 100.f), 100);
        });

        request.run();

        if (requestIsSuccessfull)
        {
            std::vector<char>& output = request.output();

            completeHandler(loader.get(), &*output.begin(), output.size());
        }
    }
#endif
}

bool
HTTPProtocol::fileExists(const std::string& filename)
{
    auto username = std::string();
    auto password = std::string();

    const std::unordered_map<std::string, std::string>* additionalHeaders = nullptr;

    auto verifyPeer = true;

    auto httpOptions = std::dynamic_pointer_cast<HTTPOptions>(_options);

    if (httpOptions != nullptr)
    {
        username = httpOptions->username();
        password = httpOptions->password();

        additionalHeaders = &httpOptions->additionalHeaders();

        verifyPeer = httpOptions->verifyPeer();
    }

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    auto evalString = std::string();

    evalString += "var xhr = new XMLHttpRequest();\n";

    evalString += "xhr.open('HEAD', '" + filename + "', false);\n";

    if (additionalHeaders != nullptr)
    {
        for (const auto& additionalHeader : *additionalHeaders)
        {
            if (additionalHeader.first == "")
                continue;

            evalString += "xhr.setRequestHeader('" + additionalHeader.first + "', '" + additionalHeader.second + "');\n";
        }
    }

    evalString += "xhr.send(null);\n";

    evalString += "(xhr.status);";

    auto status = emscripten_run_script_int(evalString.c_str());

    return (status >= 200 && status < 300);
#else
    return HTTPRequest::fileExists(filename, username, password, additionalHeaders, false);
#endif
}

bool
HTTPProtocol::isAbsolutePath(const std::string& filename) const
{
    return filename.find("://") != std::string::npos;
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
