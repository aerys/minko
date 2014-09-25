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

#include "minko/component/SceneManager.hpp"
#include "minko/log/Logger.hpp"
#include "minko/video/AndroidVideoCamera.hpp"
#include "minko/video/ImageFormat.hpp"

#include <jni.h>
#include "SDL.h"
#include <android/log.h>

using namespace minko;
using namespace minko::component;
using namespace minko::video;

struct AndroidVideoCamera::JniImpl
{
    jclass _videoCameraClass;
    jmethodID _videoCameraStartMethodID;
    jmethodID _videoCameraStopMethodID;
    jmethodID _videoCameraDesiredSizeMethodID;
    jmethodID _videoCameraRetrieveFrameMethodID;
};

static
JNIEnv*
retrieveJniEnv()
{
    return reinterpret_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
}

static
void
decodeYUVFrame(const unsigned char* frame,
			   std::vector<unsigned char>& rgbFrame,
			   int width,
			   int height)
{
	auto frameSize = width * height;
  
    for (int j = 0, yp = 0; j < height; j++) 
    {
        int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;

        for (int i = 0; i < width; i++, yp++)
        {
            int y = (0xff & ((int) frame[yp])) - 16;

            if (y < 0)
                y = 0;

            if ((i & 1) == 0)
            {  
                v = (0xff & frame[uvp++]) - 128;  
                u = (0xff & frame[uvp++]) - 128;  
            }  
    
            int y1192 = 1192 * y;  
            int r = (y1192 + 1634 * v);  
            int g = (y1192 - 833 * v - 400 * u);  
            int b = (y1192 + 2066 * u);  
    
            if (r < 0)                 
                r = 0;
            else if (r > 262143)
                r = 262143;  
            if (g < 0)                  
                g = 0;               
            else if (g > 262143)
                g = 262143;  
            if (b < 0)                  
                b = 0;               
            else if (b > 262143)
                b = 262143;  
    
            rgbFrame[yp * 3 + 0] = (unsigned char) (((r << 6) & 0xff0000) >> 16);
            rgbFrame[yp * 3 + 1] = (unsigned char) (((g >> 2) & 0xff00) >> 8);
            rgbFrame[yp * 3 + 2] = (unsigned char) (((b >> 10) & 0xff));
        }
    }  
}

const unsigned int AndroidVideoCamera::_defaultDesiredFrameRate = 30;

AndroidVideoCamera::AndroidVideoCamera() :
    _frameReceived(FrameSignal::create()),
    _desiredFrameRate(_defaultDesiredFrameRate)
{
}

void
AndroidVideoCamera::initialize()
{
    _jniImpl = std::shared_ptr<JniImpl>(new JniImpl());

    auto jniEnv = retrieveJniEnv();

	_jniImpl->_videoCameraClass = reinterpret_cast<jclass>(jniEnv->NewGlobalRef(jniEnv->FindClass("minko/plugin/video/VideoCamera")));

	_jniImpl->_videoCameraStartMethodID = jniEnv->GetStaticMethodID(_jniImpl->_videoCameraClass, "start", "()V");
	_jniImpl->_videoCameraStopMethodID = jniEnv->GetStaticMethodID(_jniImpl->_videoCameraClass, "stop", "()V");
	_jniImpl->_videoCameraDesiredSizeMethodID = jniEnv->GetStaticMethodID(_jniImpl->_videoCameraClass, "desiredSize", "(II)V");
    _jniImpl->_videoCameraRetrieveFrameMethodID = jniEnv->GetStaticMethodID(_jniImpl->_videoCameraClass, "retrieveFrame", "([I)[B");

    _frameBeginSlot = _sceneManager->frameBegin()->connect(std::bind(
        &AndroidVideoCamera::frameBeginHandler,
        std::static_pointer_cast<AndroidVideoCamera>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _frameEndSlot = _sceneManager->frameEnd()->connect(std::bind(
        &AndroidVideoCamera::frameEndHandler,
        std::static_pointer_cast<AndroidVideoCamera>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
}

AbstractVideoCamera::Ptr
AndroidVideoCamera::desiredSize(unsigned int width, unsigned int height)
{
    retrieveJniEnv()->CallStaticVoidMethod(_jniImpl->_videoCameraClass, _jniImpl->_videoCameraDesiredSizeMethodID, width, height);
}

void
AndroidVideoCamera::start()
{
    retrieveJniEnv()->CallStaticVoidMethod(_jniImpl->_videoCameraClass, _jniImpl->_videoCameraStartMethodID);
}

void
AndroidVideoCamera::stop()
{
    retrieveJniEnv()->CallStaticVoidMethod(_jniImpl->_videoCameraClass, _jniImpl->_videoCameraStopMethodID);
}

void
AndroidVideoCamera::frameBeginHandler(SceneManager::Ptr sceneManager, float time, float deltaTime)
{
    static auto elapsedTime = 0.0f;

    elapsedTime += deltaTime / 1000.0f;

    const auto videoFrameTime = 1.0f / static_cast<float>(_desiredFrameRate);

    if (elapsedTime >= videoFrameTime)
    {
        if (retrieveFrame())
        {
            elapsedTime = 0.0f;
        }
    }
}

void
AndroidVideoCamera::frameEndHandler(SceneManager::Ptr sceneManager, float time, float deltaTime)
{
}

bool
AndroidVideoCamera::retrieveFrame()
{
    static const auto frameMetaDataSize = 3;

    auto jniEnv = retrieveJniEnv();

    auto jniFrameMetaData = jniEnv->NewIntArray(frameMetaDataSize);

    auto jniFrameData = reinterpret_cast<jbyteArray>(jniEnv->CallStaticObjectMethod(
        _jniImpl->_videoCameraClass,
        _jniImpl->_videoCameraRetrieveFrameMethodID,
        jniFrameMetaData
    ));

    auto frameWasRetrieved = false;

    if (jniFrameData)
    {
        frameWasRetrieved = true;

        auto frameMetaData = jniEnv->GetIntArrayElements(jniFrameMetaData, nullptr);

        auto width = frameMetaData[0];
        auto height = frameMetaData[1];
        auto format = frameMetaData[2];

        jniEnv->ReleaseIntArrayElements(jniFrameMetaData, frameMetaData, 0);

        auto frameDataSize = jniEnv->GetArrayLength(jniFrameData);
        auto frameData = jniEnv->GetByteArrayElements(jniFrameData, nullptr);

        static const auto androidImageFormatMap = std::map<int, ImageFormatType>
        {
            { 17, ImageFormatType::NV21 }
        };
    
        const auto imageFormat = androidImageFormatMap.at(format);
        const auto numBitsPerPixel = ImageFormat::numBitsPerPixel(imageFormat);

        auto targetImageFormat = imageFormat;
        auto targetNumBitsPerPixel = ImageFormat::numBitsPerPixel(targetImageFormat);

        switch (imageFormat)
        {
        case ImageFormatType::NV21:
        {
            targetImageFormat = ImageFormatType::RGB;
            targetNumBitsPerPixel = ImageFormat::numBitsPerPixel(targetImageFormat);

            const auto ratio = targetNumBitsPerPixel / static_cast<float>(numBitsPerPixel);
    
            auto decodedFrameData = std::vector<unsigned char>(static_cast<unsigned int>(frameDataSize * ratio));
    
            decodeYUVFrame(reinterpret_cast<unsigned char*>(frameData), decodedFrameData, width, height);

            jniEnv->ReleaseByteArrayElements(jniFrameData, frameData, 0);

            jniEnv->DeleteLocalRef(jniFrameData);
            
            frameReceived()->execute(
                shared_from_this(),
                decodedFrameData,
                width,
                height,
                targetImageFormat
            );

            break;
        }
        }
    }

    jniEnv->DeleteLocalRef(jniFrameMetaData);

    return frameWasRetrieved;
}
