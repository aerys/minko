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

#include "minko/net/HTTPRequest.hpp"

#include "curl/curl.h"

using namespace minko;
using namespace minko::net;

HTTPRequest::HTTPRequest(std::string url) :
    _url(url),
    _progress(Signal<float>::create()),
    _error(Signal<int>::create()),
    _complete(Signal<const std::vector<char>&>::create())
{
}

void
HTTPRequest::run()
{
    std::cout << "HTTPRequest::run(): enter" << std::endl;

    progress()->execute(0.0f);

    std::cout << "HTTPRequest::run(): before curl init" << std::endl;

    CURL* curl = curl_easy_init();

    std::cout << "HTTPRequest::run(): after curl init" << std::endl;

    if (!curl)
        throw std::runtime_error("cURL not enabled");

    std::cout << "HTTPRequest::run(): after curl init success" << std::endl;

    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteHandler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &curlProgressHandler);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    std::cout << "HTTPRequest::run(): before curl perform" << std::endl;

    CURLcode res = curl_easy_perform(curl);

    std::cout << "HTTPRequest::run(): after curl perform" << std::endl;

    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cout << "HTTPRequest::run(): curl error" << std::endl;

        error()->execute(res);
    }
    else
    {
        std::cout << "HTTPRequest::run(): curl success" << std::endl;

        progress()->execute(1.0f);
        complete()->execute(_output);
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
