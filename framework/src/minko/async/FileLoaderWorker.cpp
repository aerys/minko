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

#include "minko/async/FileLoaderWorker.hpp"


//#if !defined(EMSCRIPTEN)
using namespace minko;
using namespace minko::async;
using namespace minko::file;

MINKO_WORKER("file-loader", FileLoaderWorker,
{
	std::cout << "FileLoaderWorker::run(): enter" << std::endl;

	std::string filename(input()->begin(), input()->end());

	output(std::make_shared<std::vector<char>>());

	Worker::MessagePtr outputData = output();

	progress(0.0);

	auto flags = std::ios::in | std::ios::ate | std::ios::binary;

	std::fstream file(filename, flags);

	if (file.is_open())
	{
		unsigned int size = (unsigned int)file.tellg();

		uint chunkSize = math::clp2((size / 50) / 1024);

		if (chunkSize > 1024)
			chunkSize = 1024;

		chunkSize *= 1024;

		std::cout << "FileLoaderWorker::run(): file is open" << std::endl;

		file.seekg(0, std::ios::beg);
		
		uint offset = 0;

		while (offset < size)
		{
			uint nextOffset = offset + chunkSize;
			uint readSize = chunkSize;

			if (nextOffset > size)
				readSize = size % chunkSize;

			outputData->resize(offset + readSize);

			file.read((char*)&(outputData->begin())[offset], readSize);

			progress((float)(offset + readSize) / (float)size);
			offset = nextOffset;
		}
		file.close();

		std::cout << "FileLoaderWorker::run(): exit" << std::endl;
	}
});
//#endif