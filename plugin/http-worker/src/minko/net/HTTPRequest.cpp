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

#include "minko/log/Logger.hpp"
#include "minko/net/HTTPRequest.hpp"

#include "curl/curl.h"

using namespace minko;
using namespace minko::net;

// From http://stackoverflow.com/questions/36746904/android-linker-undefined-reference-to-bsd-signal
#if (__ANDROID_API__ > 19)
# include <android/api-level.h>
# include <android/log.h>
# include <signal.h>
# include <dlfcn.h>

extern "C"
{
    typedef __sighandler_t (*bsd_signal_func_t)(int, __sighandler_t);
    bsd_signal_func_t bsd_signal_func = NULL;

    __sighandler_t bsd_signal(int s, __sighandler_t f)
    {
        if (bsd_signal_func == NULL)
        {
            // For now (up to Android 7.0) this is always available.
            bsd_signal_func = (bsd_signal_func_t) dlsym(RTLD_DEFAULT, "bsd_signal");

            if (bsd_signal_func == NULL)
            {
                // You may try dlsym(RTLD_DEFAULT, "signal") or dlsym(RTLD_NEXT, "signal") here
                // Make sure you add a comment here in StackOverflow
                // if you find a device that doesn't have "bsd_signal" in its libc.so!!!

                __android_log_assert("", "bsd_signal_wrapper", "bsd_signal symbol not found!");
            }
        }

        return bsd_signal_func(s, f);
      }
}
#endif

HTTPRequest::HTTPRequest(const std::string& url,
                         const std::string& username,
                         const std::string& password,
                         const std::unordered_map<std::string, std::string>* additionalHeaders) :
    _url(url),
    _progress(Signal<float>::create()),
    _error(Signal<int, const std::string&>::create()),
    _complete(Signal<const std::vector<char>&>::create()),
    _bufferSignal(Signal<const std::vector<char>&>::create()),
    _username(username),
    _password(password),
    _verifyPeer(true)
{
    if (additionalHeaders == nullptr)
        _additionalHeaders = std::unordered_map<std::string, std::string>();
    else
        _additionalHeaders = *additionalHeaders;
}

static
std::string
encodeUrl(const std::string& url)
{
    static const auto authorizedCharacters = std::set<char>
    {
         '/', ':', '~', '-', '.', '_'
    };

    std::stringstream encodedUrlStream;

    for (auto i = 0u; i < url.size(); ++i)
    {
        const auto c =  url.at(i);

        if (c == '%')
        {
            encodedUrlStream << c << url.at(i + 1) << url.at(i + 2);

            i += 2;

            continue;
        }

        if (::isalnum(c) || authorizedCharacters.find(c) != authorizedCharacters.end())
        {
            encodedUrlStream << c;
        }
        else
        {
            encodedUrlStream << "%" << std::hex << static_cast<int>(c);
        }
    }

    return encodedUrlStream.str();
}

static
CURL*
createCurl(const std::string&                                   url,
           const std::string&                                   username,
           const std::string&                                   password,
           const std::unordered_map<std::string, std::string>&  additionalHeaders,
           bool                                                 verifyPeer,
           curl_slist*&                                         curlHeaderList,
           char*                                                curlErrorBuffer)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        return nullptr;

    const auto encodedUrl = encodeUrl(url);

    curl_easy_setopt(curl, CURLOPT_URL, encodedUrl.c_str());

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifyPeer ? 1L : 0L);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // http://curl.haxx.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html
    // follow HTTP 3xx redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (!username.empty())
    {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

        const auto authenticationString = username + ":" + password;

        curl_easy_setopt(curl, CURLOPT_USERPWD, authenticationString.c_str());
    }

    if (!additionalHeaders.empty())
    {
        for (const auto& additionalHeader : additionalHeaders)
        {
            curlHeaderList = curl_slist_append(
                curlHeaderList,
                std::string(additionalHeader.first + ":" + additionalHeader.second).c_str()
            );
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaderList);
    }

    if (curlErrorBuffer != nullptr)
    {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrorBuffer);
    }

    return curl;
}

static
void
disposeCurl(CURL* curl, curl_slist* curlHeaderList)
{
    curl_easy_cleanup(curl);

    if (curlHeaderList != nullptr)
    {
        curl_slist_free_all(curlHeaderList);
    }
}

void
HTTPRequest::run()
{
    progress()->execute(0.0f);

    const auto url = _url;

    curl_slist* curlHeaderList = nullptr;

    char curlErrorBuffer[CURL_ERROR_SIZE];

    auto curl = createCurl(url, _username, _password, _additionalHeaders, _verifyPeer, curlHeaderList, curlErrorBuffer);

    if (!curl)
    {
        error()->execute(1, "failed to initialize cURL context");

        return;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteHandler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &curlProgressHandler);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

    CURLcode res = curl_easy_perform(curl);

    auto responseCode = 0l;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    disposeCurl(curl, curlHeaderList);

    const auto requestSucceeded =
        res == CURLE_OK &&
        (responseCode == 200 || responseCode == 206);

    if (!requestSucceeded)
    {
        const auto errorMessage =
            "status: " + std::to_string(responseCode) +
            (res != CURLE_OK ? ", error: " + std::string(curlErrorBuffer) : "");

        LOG_ERROR(errorMessage);

        error()->execute(responseCode, errorMessage);
    }
    else
    {
        if (buffered() && buffer().size())
        {
            bufferSignal()->execute(buffer());
            buffer(std::vector<char>());
        }

        progress()->execute(1.0f);
        complete()->execute(_output);
    }
}

size_t
HTTPRequest::curlWriteHandler(void* data, size_t size, size_t chunks, void* arg)
{
    HTTPRequest* request = static_cast<HTTPRequest*>(arg);

    size *= chunks;

    char* source = reinterpret_cast<char*>(data);

    std::vector<char>& output = request->output();

    if (request->buffered())
    {
        std::vector<char>& buffer = request->buffer();
        size_t bufferPosition = buffer.size();

        buffer.resize(bufferPosition + size);
        output.resize(size);

        std::copy(source, source + size, buffer.begin() + bufferPosition);

        if (buffer.size() > (4 * 1024 * 1024))
        {
            request->bufferSignal()->execute(buffer);

            request->buffer(std::vector<char>());
        }
    }
    else
    {
        size_t outputPosition = output.size();

        // Resizing to the new size.
        output.resize(outputPosition + size);

        // Adding the chunk to the end of the vector.
        std::copy(source, source + size, output.begin() + outputPosition);
    }

    return size;
}

int
HTTPRequest::curlProgressHandler(void* arg, double total, double current, double, double)
{
    if (total <= 0.)
        return 0;

    HTTPRequest* request = static_cast<HTTPRequest*>(arg);

    double ratio = current / total;

    request->progress()->execute(float(ratio));

    return 0;
}

bool
HTTPRequest::fileExists(const std::string& filename,
                        const std::string& username,
                        const std::string& password,
                        const std::unordered_map<std::string, std::string> *additionalHeaders,
                        bool verifyPeer)
{
    const auto url = filename;

    curl_slist* curlHeaderList = nullptr;

    char curlErrorBuffer[CURL_ERROR_SIZE];

    auto curl = createCurl(
        url,
        username,
        password,
        additionalHeaders ? *additionalHeaders : std::unordered_map<std::string, std::string>(),
        verifyPeer,
        curlHeaderList,
        curlErrorBuffer
    );

    if (!curl)
    {
        LOG_ERROR("failed to initialize cURL context");

        return false;
    }

    curl_easy_setopt(curl, CURLOPT_HEADER, false);
    curl_easy_setopt(curl, CURLOPT_NOBODY, true);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    auto status = curl_easy_perform(curl);

    auto responseCode = 0l;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    disposeCurl(curl, curlHeaderList);

    const auto requestSucceeded =
        status == CURLE_OK &&
        responseCode == 200;

    if (!requestSucceeded)
    {
        const auto errorMessage =
            "status: " + std::to_string(responseCode) +
            (status != CURLE_OK ? ", error: " + std::string(curlErrorBuffer) : "");

        LOG_ERROR(errorMessage);
    }

    return requestSucceeded;
}
