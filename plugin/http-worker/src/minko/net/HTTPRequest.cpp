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

HTTPRequest::HTTPRequest(const std::string& url,
                         const std::string& username,
                         const std::string& password,
                         const std::unordered_map<std::string, std::string>* additionalHeaders) :
    _url(url),
    _progress(Signal<float>::create()),
    _error(Signal<int>::create()),
    _complete(Signal<const std::vector<char>&>::create()),
    _username(username),
    _password(password)
{
    if (additionalHeaders == nullptr)
        _additionalHeaders = std::unordered_map<std::string, std::string>();
    else
        _additionalHeaders = *additionalHeaders;
}

void
HTTPRequest::run()
{
    progress()->execute(0.0f);

    CURL* curl = curl_easy_init();

    if (!curl)
        throw std::runtime_error("cURL not enabled");

    const auto url = _url;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteHandler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &curlProgressHandler);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	

    if (!_username.empty())
    {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

        const auto authenticationString = _username + ":" + _password;

        curl_easy_setopt(curl, CURLOPT_USERPWD, authenticationString.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    curl_slist* headerList = nullptr;

    const auto& additionalHeaders = _additionalHeaders;

    if (!additionalHeaders.empty())
    {
        for (const auto& additionalHeader : additionalHeaders)
        {
            headerList = curl_slist_append(
                headerList,
                std::string(additionalHeader.first + ":" + additionalHeader.second).c_str()
            );
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }

        CURLcode res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

    if (headerList != nullptr)
    {
        curl_slist_free_all(headerList);
    }

	    if (res != CURLE_OK)
        {
            error()->execute(res);
        }
        else
        {
            progress()->execute(1.0f);
            complete()->execute(_output);
       }
    }
	catch (std::logic_error &e)
	{
		std::cerr << "ERROR IN CURL PERFORM" << std::endl;
		return;
	}
        
}

size_t
HTTPRequest::curlWriteHandler(void* data, size_t size, size_t chunks, void* arg)
{
    HTTPRequest* request = static_cast<HTTPRequest*>(arg);

    size *= chunks;

    std::vector<char>& output = request->output();

    size_t position = output.size();

    // Resizing to the new size.
    output.resize(position + size);

    char* source = reinterpret_cast<char*>(data);

    // Adding the chunk to the end of the vector.
    std::copy(source, source + size, output.begin() + position);

    return size;
}

int
HTTPRequest::curlProgressHandler(void* arg, double total, double current, double, double)
{
    HTTPRequest* request = static_cast<HTTPRequest*>(arg);

    double ratio = current / total;

    request->progress()->execute(float(ratio));

    return 0;
}

bool
HTTPRequest::fileExists(const std::string& filename,
                        const std::string& username,
                        const std::string& password,
                        const std::unordered_map<std::string, std::string> *additionalHeaders)
{
    auto curl = curl_easy_init();

    if (!curl)
    {
        LOG_ERROR("cURL is not enabled");

        return false;
    }

    const auto url = filename;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_HEADER, false);
    curl_easy_setopt(curl, CURLOPT_NOBODY, true);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    if (!username.empty())
    {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

        const auto authenticationString = username + ":" + password;

        curl_easy_setopt(curl, CURLOPT_USERPWD, authenticationString.c_str());
    }

    curl_slist* headerList = nullptr;

    if (additionalHeaders)
    {
        for (const auto& additionalHeader : *additionalHeaders)
        {
            headerList = curl_slist_append(
                headerList,
                std::string(additionalHeader.first + ":" + additionalHeader.second).c_str()
            );
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }

    auto status = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    if (headerList != nullptr)
    {
        curl_slist_free_all(headerList);
    }

    return status == CURLE_OK;
}
