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

#include "minko/net/HTTPWorker.hpp"
#include "minko/net/HTTPRequest.hpp"

using namespace minko;
using namespace minko::net;

namespace minko
{
    namespace net
    {
        typedef std::unordered_map<std::string, std::string> StringStringUnorderedMap;

        MINKO_DEFINE_WORKER(HTTPWorker,
        {
            std::stringstream inputStream(std::string(input.begin(), input.end()));

            auto urlSize = 0;
            auto usernameSize = 0;
            auto passwordSize = 0;
            auto numAdditionalHeaders = 0;
            auto verifyPeer = true;
            auto buffered = false;

            inputStream.read(reinterpret_cast<char*>(&urlSize), 4);
            auto urlData = std::vector<char>(urlSize);

            if (urlSize > 0)
                inputStream.read(urlData.data(), urlSize);

            inputStream.read(reinterpret_cast<char*>(&usernameSize), 4);
            auto usernameData = std::vector<char>(usernameSize);

            if (usernameSize > 0)
                inputStream.read(usernameData.data(), usernameSize);

            inputStream.read(reinterpret_cast<char*>(&passwordSize), 4);
            auto passwordData = std::vector<char>(passwordSize);

            if (passwordSize > 0)
                inputStream.read(passwordData.data(), passwordSize);

            const auto url = std::string(urlData.begin(), urlData.end());
            const auto username = std::string(usernameData.begin(), usernameData.end());
            const auto password = std::string(passwordData.begin(), passwordData.end());

            inputStream.read(reinterpret_cast<char*>(&numAdditionalHeaders), 4);

            StringStringUnorderedMap additionalHeaders;

            for (auto i = 0; i < numAdditionalHeaders; ++i)
            {
                auto keySize = 0;
                auto valueSize = 0;

                inputStream.read(reinterpret_cast<char*>(&keySize), 4);
                inputStream.read(reinterpret_cast<char*>(&valueSize), 4);

                auto keyData = std::vector<char>(keySize);
                auto valueData = std::vector<char>(valueSize);

                if (keySize > 0)
                    inputStream.read(keyData.data(), keySize);

                if (valueSize > 0)
                    inputStream.read(valueData.data(), valueSize);

                additionalHeaders.insert(std::make_pair(
                    std::string(keyData.begin(), keyData.end()),
                    std::string(valueData.begin(), valueData.end())
                ));
            }

            inputStream.read(reinterpret_cast<char*>(&verifyPeer), 1);
            inputStream.read(reinterpret_cast<char*>(&buffered), 1);

            HTTPRequest request(url, username, password, additionalHeaders.empty() ? nullptr : &additionalHeaders);

            request.verifyPeer(verifyPeer);
            request.buffered(buffered);

            auto _0 = request.progress()->connect([&](float p) {
                Message message { "progress" };
                message.set(p);
                post(message);
            });

            auto _1 = request.error()->connect([&](int e, const std::string& errorMessage) {
                post(Message { "error" });
            });

            auto _2 = request.complete()->connect([&](const std::vector<char>& output) {
                Message message{ "complete" };
                message.set(output);
                post(message);
            });

            auto _3 = request.bufferSignal()->connect([&](const std::vector<char>& buffer) {
                Message message{ "buffer" };
                message.set(buffer);
                post(message);
            });

            request.run();
        });
    }
}
