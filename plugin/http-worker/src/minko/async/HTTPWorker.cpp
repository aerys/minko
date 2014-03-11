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

#include "minko/async/HTTPWorker.hpp"

using namespace minko;
using namespace minko::async;
using namespace minko::file;

#if defined(EMSCRIPTEN)
# error "HTTPWorker should not be used in HTML5"
#endif

#include "curl/curl.h"

MINKO_WORKER("http", minko::async::HTTPWorker, {
	std::cout << "HTTPWorker::run(): enter" << std::endl;

	std::string filename(input()->begin(), input()->end());

	output(std::make_shared<std::vector<char>>());

	std::cout << "HTTPWorker::run(): before curl init" << std::endl;

	CURL* curl = curl_easy_init();

	std::cout << "HTTPWorker::run(): after curl init" << std::endl;

	if (!curl)
		throw std::runtime_error("cURL not enabled");

	std::cout << "HTTPWorker::run(): after curl init success" << std::endl;

	curl_easy_setopt(curl, CURLOPT_URL, filename.c_str());

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteHandler);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &curlProgressHandler);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	std::cout << "HTTPWorker::run(): before curl perform" << std::endl;

	CURLcode res = curl_easy_perform(curl);

	std::cout << "HTTPWorker::run(): after curl perform" << std::endl;

	if (res != CURLE_OK)
	{
		std::cout << "HTTPWorker::run(): curl error" << std::endl;
		// FIXME: Deal with errors.
		// errorHandler(loader.get());
	}

	curl_easy_cleanup(curl);

	std::cout << "HTTPWorker::run(): exit" << std::endl;
});

namespace minko
{
	namespace async
	{
		size_t
		HTTPWorker::curlWriteHandler(void* data, size_t size, size_t chunks, void* arg)
		{
			minko::async::HTTPWorker* worker = static_cast<minko::async::HTTPWorker*>(arg);

			size *= chunks;

			Worker::MessagePtr output = worker->output();

			size_t position = output->size();

			// Resizing to the new size.
			output->resize(position + size);

			char* source = reinterpret_cast<char*>(data);

			// Adding the chunk to the end of the vector.
			std::copy(source, source + size, output->begin() + position);

			return size;
		}

		int
		HTTPWorker::curlProgressHandler(void* arg, double total, double current, double, double)
		{
			minko::async::HTTPWorker* worker = static_cast<minko::async::HTTPWorker*>(arg);

			double ratio = current / total;

			worker->progress(float(ratio));

			return 0;
		}
	}
}
