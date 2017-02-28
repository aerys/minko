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

#include "minko/file/AndroidUnzip.hpp"
#include "minko/log/Logger.hpp"

#include "SDL.h"

#include "jni.h"

using namespace minko;
using namespace minko::file;

std::string
AndroidUnzip::extractFromAsset(const std::string& path)
{
    auto env = (JNIEnv*) SDL_AndroidGetJNIEnv();
    jobject sdlActivity = (jobject) SDL_AndroidGetActivity();
    jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

    jclass androidUnzipClass = env->FindClass("minko/file/AndroidUnzip");
    jmethodID androidUnzipCtor = env->GetMethodID(androidUnzipClass, "<init>", "(Landroid/app/Activity;)V");
    jobject androidUnzipObject = env->NewObject(androidUnzipClass, androidUnzipCtor, sdlActivity);

    jstring inputPathString = env->NewStringUTF(path.c_str());

    jmethodID extractFromAsset = env->GetMethodID(androidUnzipClass, "extractFromAsset", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring outputPathString = (jstring) env->CallObjectMethod(androidUnzipObject, extractFromAsset, inputPathString);

    const char* outputPathChars = env->GetStringUTFChars(outputPathString, nullptr);
    std::string extractDir(outputPathChars);

    env->ReleaseStringUTFChars(outputPathString, outputPathChars);

    return extractDir;
}
