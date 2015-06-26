/*
Copyright (c) 2015 Aerys

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
#include "minko/video/ios/IOSVideoCamera.hpp"
#include "minko/video/ImageFormat.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::video;


IOSVideoCamera::IOSVideoCamera() :
    _videoSourceImpl(new VideoSourceImpl()),
    _frameReceived(FrameSignal::create())
{
}

IOSVideoCamera::~IOSVideoCamera()
{
    delete _videoSourceImpl;
}

void
IOSVideoCamera::initialize()
{
    _iOSFrameReceivedSlot = _videoSourceImpl->frameReceived->connect(
    [&](const std::vector<unsigned char>& data, int width, int height)
    {
        if (_imageBuffer.width != width || _imageBuffer.height != height)
            _rgbaFrame = std::vector<unsigned char>((width * height) * 4);
        
        _imageBuffer.data = data;
        _imageBuffer.width = width;
        _imageBuffer.height = height;
    });
    
    _videoSourceImpl->initialize();
}

AbstractVideoCamera::Ptr
IOSVideoCamera::desiredSize(unsigned int width, unsigned int height)
{
    return shared_from_this();
}

void
IOSVideoCamera::start()
{
    _videoSourceImpl->start();
}

void
IOSVideoCamera::stop()
{
    _videoSourceImpl->stop();
}

void
IOSVideoCamera::decodeBGRAFrame(const unsigned char* data,
                                std::vector<unsigned char>& rgbaFrame,
                                int width,
                                int height)
{
    if (rgbaFrame.size() == 0)
        return;
    
    int offset = 0;
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            rgbaFrame[offset] = data[offset + 2];
            rgbaFrame[offset + 1] = data[offset + 1];
            rgbaFrame[offset + 2] = data[offset];
            rgbaFrame[offset + 3] = data[offset + 3];
            
            offset += 4;
        }
    }
}

void
IOSVideoCamera::requestFrame()
{
    // We retrieve a BGRA image buffer, so we need to convert it into RGBA format
    decodeBGRAFrame(reinterpret_cast<unsigned char*>(_imageBuffer.data.data()), _rgbaFrame, _imageBuffer.width, _imageBuffer.height);
    
    _frameReceived->execute(shared_from_this(), _rgbaFrame, _imageBuffer.width, _imageBuffer.height, ImageFormatType::RGBA);
}
