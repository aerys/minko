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

#include "minko/file/APKProtocolWorker.hpp"
#include "minko/log/Logger.hpp"

#include "SDL_rwops.h"

using namespace minko;
using namespace minko::file;

namespace minko
{
	namespace file
	{
		MINKO_DEFINE_WORKER(APKProtocolWorker,
		{
            std::stringstream inputStream(std::string(input.begin(), input.end()));

            auto seekingOffset = 0;
            auto seekedLength = 0;
            auto filename = std::string(input.begin() + 8u, input.end());

            inputStream.read(reinterpret_cast<char*>(&seekingOffset), 4u);
            inputStream.read(reinterpret_cast<char*>(&seekedLength), 4u);

			std::vector<char> output;

			post(Message { "progress" }.set(0.0f));

            SDL_RWops* file = SDL_RWFromFile(filename.c_str(), "rb");

			if (file)
			{
				uint length = seekedLength > 0 ? seekedLength : (uint(file->size(file)) - seekingOffset);

				uint chunkSize = math::clp2((length / 50) / 1024);

				if (chunkSize > 1024)
					chunkSize = 1024;
				else if (chunkSize <= 0)
					chunkSize = 8;

				chunkSize *= 1024;

				file->seek(file, seekingOffset, RW_SEEK_SET);

				uint offset = 0;

				while (offset < length)
				{
					uint nextOffset = offset + chunkSize;
					uint readSize = chunkSize;

					if (nextOffset > length)
						readSize = length % chunkSize;

					output.resize(offset + readSize);

					file->read(file, &*output.begin() + offset, readSize, 1u);

                    auto progress = float(offset + readSize) / float(length);

                    progress *= 100.0;

                    post(Message { "progress" }.set(progress));

					offset = nextOffset;
				}

				file->close(file);

				post(Message{ "complete" }.set(output));
			}
			else
			{
				post(Message{ "error" });
			}
		});
	}
}
