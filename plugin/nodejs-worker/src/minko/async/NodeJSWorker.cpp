/*
Copyright (c) 2016 Aerys

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

#include "minko/async/NodeJSWorker.hpp"
#include "minko/log/Logger.hpp"

#include "node/node.h"

#include <unistd.h>

using namespace minko;
using namespace minko::async;

namespace minko
{
    namespace async
    {
        static
        char **
        makeArgvCopy(int argc, char** argv)
        {
            // From: https://gist.github.com/bnoordhuis/1981730.
            size_t strlen_sum;
            char **argp;
            char *data;
            size_t len;
            int i;

            strlen_sum = 0;
            for (i = 0; i < argc; i++) strlen_sum += strlen(argv[i]) + 1;

            argp = (char**) malloc(sizeof(char *) * (argc + 1) + strlen_sum);
            if (!argp) return NULL;
            data = (char *) argp + sizeof(char *) * (argc + 1);

            for (i = 0; i < argc; i++) {
                argp[i] = data;
                len = strlen(argv[i]) + 1;
                memcpy(data, argv[i], len);
                data += len;
            }
            argp[argc] = NULL;

            return argp;
        }

        static
        void
        startNode(const std::string& input)
        {
            std::stringstream inputStream(std::string(input.begin(), input.end()));

            auto nodeArgsSize = 0;
            auto pathSize     = 0;
            auto argsSize     = 0;

            inputStream.read(reinterpret_cast<char*>(&nodeArgsSize), 4);
            auto nodeArgsData = std::vector<char>(nodeArgsSize);

            if (nodeArgsSize > 0)
                inputStream.read(nodeArgsData.data(), nodeArgsSize);

            const auto nodeArgs = std::string(nodeArgsData.begin(), nodeArgsData.end());

            inputStream.read(reinterpret_cast<char*>(&pathSize), 4);
            auto pathData = std::vector<char>(pathSize);

            if (pathSize > 0)
                inputStream.read(pathData.data(), pathSize);

            const auto path = std::string(pathData.begin(), pathData.end());

            inputStream.read(reinterpret_cast<char*>(&argsSize), 4);

            auto argsData = std::vector<char>(argsSize);

            if (argsSize > 0)
                inputStream.read(argsData.data(), argsSize);

            auto args = std::string(argsData.begin(), argsData.end());

            std::string binary = "node";
            std::string scriptPath = path + "/" + "index.js";

            std::vector<std::string> argsVec;

            argsVec.push_back(binary);
            if (nodeArgsSize > 0) {
                argsVec.push_back(nodeArgs);
            }
            argsVec.push_back(scriptPath);

            std::string token = " ";
            auto i = args.find(token);

            while (i != -1)
            {
                argsVec.push_back(args.substr(0, i));

                args = args.substr(i + token.size());

                i = args.find(token);
            }

            argsVec.push_back(args);

            chdir(path.c_str());

            char ** argsList = new char*[argsVec.size() + 1];

            for(size_t i = 0; i < argsVec.size(); i++)
            {
                argsList[i] = new char[argsVec[i].size() + 1];
                strcpy(argsList[i], argsVec[i].c_str());
            }

            argsList[argsVec.size()] = nullptr;

            int argc = argsVec.size();
            char** argv = makeArgvCopy(argc, argsList);

            LOG_INFO("Start node on " << scriptPath);
            node::Start(argc, argv);
        }

        MINKO_DEFINE_WORKER(NodeJSWorker,
        {
            std::string path(input.begin(), input.end());

            startNode(path);
        });
    }
}
