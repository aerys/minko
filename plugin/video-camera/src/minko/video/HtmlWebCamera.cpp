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
#include "minko/video/HtmlWebCamera.hpp"
#include "minko/video/ImageFormat.hpp"

#include "SDL.h"

#include "minko/log/Logger.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#endif

using namespace minko;
using namespace minko::component;
using namespace minko::video;

bool HtmlWebCamera::_js_script_loaded(false);


HtmlWebCamera::HtmlWebCamera() :
    _frameReceived(FrameSignal::create()),
    _frameId(0)
{
}


void
HtmlWebCamera::js_load_success()
{
    _js_script_loaded = true;
}

void
HtmlWebCamera::js_load_error()
{
    throw file::Error("A JS script contains errors (HtmlWebCamera)");
}


void
HtmlWebCamera::initialize()
{
#if defined(EMSCRIPTEN)
    emscripten_async_load_script("asset/script/minko.camera.js", 
        js_load_success,
        js_load_error
    );
#endif
}

AbstractVideoCamera::Ptr
HtmlWebCamera::desiredSize(unsigned int width, unsigned int height)
{
    return shared_from_this();
}

void
HtmlWebCamera::start()
{
}

void
HtmlWebCamera::stop()
{
}

void 
HtmlWebCamera::requestFrame()
{
    //Call Js method
    if (_js_script_loaded)
    {
        if (_frameId == 0) //first frame
        {
#if defined(EMSCRIPTEN)
        std::string eval = "(minko_camera.width)";
        _cameraWidth = emscripten_run_script_int(eval.c_str());
        eval = "(minko_camera.height)";
        _cameraHeight = emscripten_run_script_int(eval.c_str());
        eval = "(minko_camera.bufSize)";
        _cameraBufferSize = emscripten_run_script_int(eval.c_str());
#endif
        }
        int tempFrameId = 0;
        unsigned char* cameraBuffer;
        
#if defined(EMSCRIPTEN)
        //emscripten_run_script_string("return_array();");
        std::string eval = "(minko_camera.webcamBuffer)";
        cameraBuffer = (unsigned char*)emscripten_run_script_int(eval.c_str());
        eval = "(minko_camera.frameId)";
        tempFrameId = emscripten_run_script_int(eval.c_str());
#endif
        if (tempFrameId != _frameId)
        {
            _frameId = tempFrameId;

            auto targetImageFormat = ImageFormatType::RGBA;
            auto targetNumBitsPerPixel = ImageFormat::numBitsPerPixel(targetImageFormat);

            frameReceived()->execute(
                shared_from_this(),
                std::vector<unsigned char>(cameraBuffer, cameraBuffer + _cameraBufferSize),
                _cameraWidth,
                _cameraHeight,
                targetImageFormat
            );
        }
    }
}

