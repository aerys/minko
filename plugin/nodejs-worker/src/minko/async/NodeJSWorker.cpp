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

#include "node.h"

#include <unistd.h>

using namespace minko;
using namespace minko::async;

namespace minko
{
    namespace async
    {
        static
        char **
        makeArgvCopy(int argc, const char** argv)
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
        startNode(const std::string& path)
        {
            chdir(path.c_str());

            std::string binary = "node";
            std::string scriptPath = path + "/" + "index.js";

            const char* args[] = { binary.c_str() , scriptPath.c_str(), nullptr };

            int argc = 2;
            char** argv = makeArgvCopy(argc, args);

            LOG_DEBUG("Start node on " << scriptPath);
            node::Start(argc, argv);
        }

        MINKO_DEFINE_WORKER(NodeJSWorker,
        {
            std::string path(input.begin(), input.end());

            startNode(path);
        });
    }
}
