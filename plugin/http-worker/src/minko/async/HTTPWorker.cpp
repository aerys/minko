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
	std::string url(input()->begin(), input()->end());

	output(std::make_shared<std::vector<char>>());

	auto helper = new HTTPWorkerHelper(url, output());

	helper->progress()->connect([&](float p){
		progress(p);
	});

	helper->run();
});

namespace minko
{
	namespace async
	{
		HTTPWorkerHelper::HTTPWorkerHelper(std::string url, Worker::MessagePtr output) :
			_url(url),
			_output(output),
			_progress(Signal<float>::create())
		{
		}

		void
		HTTPWorkerHelper::run()
		{
			std::cout << "HTTPWorkerHelper::run(): enter" << std::endl;

			progress()->execute(0.0f);

			std::cout << "HTTPWorkerHelper::run(): before curl init" << std::endl;

			CURL* curl = curl_easy_init();

			std::cout << "HTTPWorkerHelper::run(): after curl init" << std::endl;

			if (!curl)
				throw std::runtime_error("cURL not enabled");
			std::cout << "HTTPWorkerHelper::run(): after curl init success" << std::endl;

			curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteHandler);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &curlProgressHandler);
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

			curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

			std::cout << "HTTPWorkerHelper::run(): before curl perform" << std::endl;

			CURLcode res = curl_easy_perform(curl);

			std::cout << "HTTPWorkerHelper::run(): after curl perform" << std::endl;

			if (res != CURLE_OK)
			{
				std::cout << "HTTPWorkerHelper::run(): curl error" << std::endl;
				// FIXME: Deal with errors.
				// errorHandler(loader.get());
			}

			curl_easy_cleanup(curl);

			progress()->execute(1.0f);

			std::cout << "HTTPWorkerHelper::run(): exit" << std::endl;
		}

		size_t
		HTTPWorkerHelper::curlWriteHandler(void* data, size_t size, size_t chunks, void* arg)
		{
			minko::async::HTTPWorkerHelper* helper = static_cast<minko::async::HTTPWorkerHelper*>(arg);

			size *= chunks;

			Worker::MessagePtr output = helper->output();

			size_t position = output->size();

			// Resizing to the new size.
			output->resize(position + size);

			char* source = reinterpret_cast<char*>(data);

			// Adding the chunk to the end of the vector.
			std::copy(source, source + size, output->begin() + position);

			return size;
		}

		int
		HTTPWorkerHelper::curlProgressHandler(void* arg, double total, double current, double, double)
		{
			minko::async::HTTPWorkerHelper* helper = static_cast<minko::async::HTTPWorkerHelper*>(arg);

			double ratio = current / total;

			helper->progress()->execute(float(ratio));

			return 0;
		}
	}
}
