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

#ifdef __ANDROID__
#include "APKLoader.hpp"

#include "Options.hpp"
#include "minko/Signal.hpp"

#include <fstream>

#include <jni.h>

using namespace minko;
using namespace minko::file;

JavaVM* APKLoader::jvm;

extern "C"
{
    jint JNI_OnLoad(JavaVM *vm, void *reserved)
    {
        APKLoader::jvm = vm;
        return JNI_VERSION_1_2;
    }
}

APKLoader::APKLoader()
{
}

void APKLoader::load(const std::string& filename, std::shared_ptr<Options> options)
{
    _filename = filename;
    _resolvedFilename = filename;
    _options = options;

    JNIEnv* env;
    JavaVM* jvm = APKLoader::jvm;
    jvm->AttachCurrentThread(&env, nullptr);

    jstring jstr = env->NewStringUTF(filename.c_str());
    jclass clazz = env->FindClass("in/aerys/minko/example/cube/AssetsLoader");
    jmethodID method = env->GetStaticMethodID(clazz, "LoadAsset", "(Ljava/lang/String;)[B");
    jbyteArray retval = (jbyteArray) env->CallStaticObjectMethod(clazz, method, jstr);

    if (nullptr != retval)
    {
        _progress->execute(shared_from_this());

        int len = env->GetArrayLength(retval);
        unsigned char* buf = new unsigned char[len];
        env->GetByteArrayRegion(retval, 0, len, reinterpret_cast<jbyte*>(buf));

        _data.assign(buf, buf + len);

        _progress->execute(shared_from_this());

        _complete->execute(shared_from_this());
    }
    else
        _error->execute(shared_from_this());
}
#endif /* __ANDROID__ */
