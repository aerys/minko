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

#include "minko/file/FileProtocolWorker.hpp"

using namespace minko;
using namespace minko::file;

namespace minko
{
	namespace file
	{
		MINKO_DEFINE_WORKER(FileProtocolWorker,
		{
            auto seekingOffset = (static_cast<int>(static_cast<unsigned char>(input[0])) << 24) +
                                 (static_cast<int>(static_cast<unsigned char>(input[1])) << 16) +
                                 (static_cast<int>(static_cast<unsigned char>(input[2])) << 8) +
                                 static_cast<int>(static_cast<unsigned char>(input[3]));

            auto seekedLength = (static_cast<int>(static_cast<unsigned char>(input[4])) << 24) +
                                (static_cast<int>(static_cast<unsigned char>(input[5])) << 16) +
                                (static_cast<int>(static_cast<unsigned char>(input[6])) << 8) +
                                static_cast<int>(static_cast<unsigned char>(input[7]));

			std::string filename(input.begin() + 8, input.end());

			std::vector<char> output;

			post(Message { "progress" }.set(0.0f));

			auto flags = std::ios::in | std::ios::ate | std::ios::binary;

			std::fstream file(filename, flags);

			if (file.is_open())
			{
				uint length = seekedLength > 0 ? seekedLength : (uint(file.tellg()) - seekingOffset);

				uint chunkSize = math::clp2((length / 50) / 1024);

				if (chunkSize > 1024)
					chunkSize = 1024;
				else if (chunkSize <= 0)
					chunkSize = 8;

				chunkSize *= 1024;

				file.seekg(seekingOffset, std::ios::beg);

				uint offset = 0;

				while (offset < length)
				{
					uint nextOffset = offset + chunkSize;
					uint readSize = chunkSize;

					if (nextOffset > length)
						readSize = length % chunkSize;

					output.resize(offset + readSize);

					file.read(&*output.begin() + offset, readSize);

                    auto progress = float(offset + readSize) / float(length);

                    progress *= 100.0;

                    post(Message { "progress" }.set(progress));

					offset = nextOffset;
				}

				file.close();

				post(Message{ "complete" }.set(output));
			}
			else
			{
				post(Message{ "error" });
			}
		});
	}
}
