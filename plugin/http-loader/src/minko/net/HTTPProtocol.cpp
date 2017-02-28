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

using namespace minko;
using namespace minko::file;
using namespace minko::async;
using namespace minko::net;

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"

extern "C" {
    extern int emscripten_async_wget3_data(const char* url, const char* requesttype, const char* param, const char* additionalHeader, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress);
}
#endif

std::unordered_set<HTTPProtocol::Ptr> HTTPProtocol::_httpProtocolReferences;

HTTPProtocol::HTTPProtocol() :
    AbstractProtocol(),
    _status(Options::FileStatus::Pending)
{
}

void
HTTPProtocol::progressHandler(int loadedBytes, int totalBytes)
{
    if (_status == Options::FileStatus::Aborted)
        return;

    float progress = 0.0;

    if (totalBytes != 0)
        progress = (float)(loadedBytes) / (float)(totalBytes);

#if defined(EMSCRIPTEN)
    if (options()->fileStatusFunction())
    {
        const auto fileStatus = options()->fileStatusFunction()(file(), progress);

        if (fileStatus == Options::FileStatus::Aborted)
        {
            _status = Options::FileStatus::Aborted;

            emscripten_async_wget2_abort(_handle);

            error()->execute(shared_from_this());

            return;
        }
    }
#endif

    this->progress()->execute(shared_from_this(), progress);
}

void
HTTPProtocol::completeHandler(void* data, unsigned int size)
{
    if (_status == Options::FileStatus::Aborted)
        return;

    this->data().assign(static_cast<unsigned char*>(data), static_cast<unsigned char*>(data) + size);

    progress()->execute(shared_from_this(), 1.0);
    complete()->execute(shared_from_this());

    _httpProtocolReferences.erase(std::static_pointer_cast<HTTPProtocol>(shared_from_this()));
}

void
HTTPProtocol::bufferHandler(const void* data, unsigned int size)
{
    if (_status == Options::FileStatus::Aborted)
        return;

    this->fileBuffer().clear();
    this->fileBuffer().assign(reinterpret_cast<const unsigned char*>(data), reinterpret_cast<const unsigned char*>(data) + size);

    buffer()->execute(shared_from_this());
}

void
HTTPProtocol::errorHandler(int code, const char * message)
{
    LOG_ERROR(message);

    error()->execute(shared_from_this());

    _httpProtocolReferences.erase(std::static_pointer_cast<HTTPProtocol>(shared_from_this()));
}

void
HTTPProtocol::load()
{
    _httpProtocolReferences.insert(std::static_pointer_cast<HTTPProtocol>(shared_from_this()));

    _options->protocolFunction([](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
    {
        return HTTPProtocol::create();
    });

    progress()->execute(shared_from_this(), 0.0);

    auto username = std::string();
    auto password = std::string();
    auto additionalHeaders = std::unordered_map<std::string, std::string>();
    auto verifyPeer = true;
    auto buffered = _options->buffered();

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

        _handle = emscripten_async_wget3_data(
            resolvedFilename().c_str(),
            "GET",
            "",
            additionalHeadersJsonString.c_str(),
            this,
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

            completeHandler((void*)bytes, size);
        }
        else
        {
            errorHandler();
        }
    }
#else
    if (options()->loadAsynchronously())
    {
        auto worker = AbstractCanvas::defaultCanvas()->getWorker("http");

        _workerSlots.push_back(worker->message()->connect([=](Worker::Ptr, Worker::Message message) {
            if (message.type == "complete")
            {
                completeHandler(&*message.data.begin(), message.data.size());
            }
            else if (message.type == "progress")
            {
                float ratio = *reinterpret_cast<float*>(&*message.data.begin());
                progressHandler(int(ratio * 10000.f), 10000);
            }
            else if (message.type == "error")
            {
                errorHandler();
            }
            else if (message.type == "buffer")
            {
                bufferHandler(message.data.data(), message.data.size());
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
        inputStream.write(reinterpret_cast<const char*>(&buffered), 1);

        auto inputString = inputStream.str();

        worker->start(std::vector<char>(inputString.begin(), inputString.end()));
    }
    else
    {
        HTTPRequest request(resolvedFilename(), username, password, &additionalHeaders);

        request.verifyPeer(verifyPeer);
        request.verifyPeer(buffered);

        auto requestIsSuccessfull = true;

        auto requestErrorSlot = request.error()->connect([&](int error, const std::string& errorMessage)
        {
            requestIsSuccessfull = false;

            this->error()->execute(shared_from_this());
        });

        auto requestProgressSlot = request.progress()->connect([&](float p) {
            progressHandler(int(p * 100.f), 100);
        });

        auto bufferSlot = request.bufferSignal()->connect([&](const std::vector<char>& buffer) {
            bufferHandler(buffer.data(), buffer.size());
        });

        request.run();

        if (requestIsSuccessfull)
        {
            std::vector<char>& output = request.output();

            completeHandler(&*output.begin(), output.size());
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

#if defined(EMSCRIPTEN)
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
    int fileSize = 0;
    return HTTPRequest::fileExists(filename, fileSize, username, password, additionalHeaders, false);
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
    static_cast<HTTPProtocol*>(arg)->completeHandler(data, size);
}

void
HTTPProtocol::wget2ErrorHandler(unsigned int id, void* arg, int code, const char* message)
{
    static_cast<HTTPProtocol*>(arg)->errorHandler(code, message);
}

void
HTTPProtocol::wget2ProgressHandler(unsigned int id, void* arg, int loadedBytes, int totalBytes)
{
    static_cast<HTTPProtocol*>(arg)->progressHandler(loadedBytes, totalBytes);
}
#endif
