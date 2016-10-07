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

#include "v8.h"
#include "uv.h"
#include "node.h"

#include "SDL.h"

#include "jni.h"

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
        std::string
        copyAsset(const std::string& path, const std::string& tempDir)
        {
            std::string newPath = tempDir + "/" + path;

            SDL_RWops* srcFile = SDL_RWFromFile(path.c_str(), "rb");
            SDL_RWops* destFile = SDL_RWFromFile(newPath.c_str(), "w");

            Sint64 bufferSize = SDL_RWsize(srcFile);
            char* buffer = new char[bufferSize + 1];

            Sint64 size = 1;
            while ((size = SDL_RWread(srcFile, buffer, 1, bufferSize)) != 0)
                SDL_RWwrite(destFile, buffer, 1, size);

            delete buffer;

            SDL_RWclose(srcFile);
            SDL_RWclose(destFile);

            return newPath;
        }

        static
        std::string
        createAndroidTemporaryDirectory()
        {
            auto env = (JNIEnv*) SDL_AndroidGetJNIEnv();
            jobject sdlActivity = (jobject) SDL_AndroidGetActivity();
            jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

            jmethodID getCacheDir = env->GetMethodID(sdlActivityClass, "getCacheDir", "()Ljava/io/File;");
            jobject cacheDir = env->CallObjectMethod(sdlActivity, getCacheDir);

            jclass fileClass = env->FindClass("java/io/File");
            jmethodID getPath = env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");
            jstring pathString = (jstring) env->CallObjectMethod(cacheDir, getPath);

            const char* pathChars = env->GetStringUTFChars(pathString, nullptr);
            std::string path(pathChars);

            env->ReleaseStringUTFChars(pathString, pathChars);

            return path;
        }

        static
        void
        startNode(const std::string& path)
        {
            std::string tempDir = createAndroidTemporaryDirectory();
            std::string newPath = copyAsset(path, tempDir);

            const char* args[] = { "node" , newPath.c_str(), nullptr };

            int argc = 2;
            char** argv = makeArgvCopy(argc, args);

            node::Start(argc, argv);
        }

        MINKO_DEFINE_WORKER(NodeJSWorker,
        {
            std::string path(input.begin(), input.end());

            startNode(path);
        });
    }
}
