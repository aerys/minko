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

#include "minko/MinkoSerializer.hpp"
#include "minko/oculus/WebVROculus.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/math/Matrix4x4.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;

WebVROculus::WebVROculus(int viewportWidth, int viewportHeight, float zNear, float zFar) :
_aspectRatio((float)viewportWidth / (float)viewportHeight),
_zNear(zNear),
_zFar(zFar),
_leftRenderer(nullptr),
_rightRenderer(nullptr),
_leftCameraNode(nullptr),
_rightCameraNode(nullptr)
{
}

void
WebVROculus::initializeOVRDevice()
{
    _leftRenderer = Renderer::create();
    _rightRenderer = Renderer::create();

    // Create renderer for each eye
    _rightRenderer->clearBeforeRender(false);

    _leftRenderer->viewport(
        0,
        0,
        960,
        1080
        );


    _rightRenderer->viewport(
        960,
        0,
        960,
        1080
        );

    std::string eval = "";

    eval += "function vrDeviceCallback(vrdevs) { alert('Callback!');                             \n";
    eval += "    for (var i = 0; i < vrdevs.length; ++i) {                   \n";
    eval += "        if (vrdevs[i] instanceof HMDVRDevice) {                 \n";
    eval += "            alert('Set vrHMD');vrHMD = vrdevs[i];alert(vrHMD);                                  \n";
    eval += "            break;                                              \n";
    eval += "        }                                                       \n";
    eval += "    }                                                           \n";
    eval += "    for (var i = 0; i < vrdevs.length; ++i) {                   \n";
    eval += "        if (vrdevs[i] instanceof PositionSensorVRDevice &&      \n";
    eval += "            vrdevs[i].hardwareUnitId == vrHMD.hardwareUnitId) { \n";
    eval += "            vrHMDSensor = vrdevs[i];                            \n";
    eval += "            break;                                              \n";
    eval += "        }                                                       \n";
    eval += "    }                                                           \n";
    eval += "}                                                               \n";
    eval += "                                                                \n";
    eval += "    if (navigator.getVRDevices) {                               \n";
    eval += "        navigator.getVRDevices().then(vrDeviceCallback);        \n";
    eval += "    } else if (navigator.mozGetVRDevices) {                     \n";
    eval += "        navigator.mozGetVRDevices(vrDeviceCallback);            \n";
    eval += "    }                                                           \n";
    eval += "                                                                \n";
    eval += "window.addEventListener(\"keydown\", function(e) {              \n";
    eval += "        var renderCanvas = document.getElementById('canvas');   \n";
    eval += "        if (renderCanvas.mozRequestFullScreen) {                \n";
    eval += "            renderCanvas.mozRequestFullScreen({                 \n";
    eval += "                vrDisplay: vrHMD                                \n";
    eval += "            });                                                 \n";
    eval += "        } else if (renderCanvas.webkitRequestFullscreen) {      \n";
    eval += "            renderCanvas.webkitRequestFullscreen({              \n";
    eval += "                vrDisplay: vrHMD,                               \n";
    eval += "            });                                                 \n";
    eval += "        }                                                       \n";
    eval += "}, false);                                                      \n";

    emscripten_run_script(eval.c_str());
}

void
WebVROculus::initializeCameras(scene::Node::Ptr target)
{
    auto leftCamera = PerspectiveCamera::create(
        _aspectRatio,
        0.78f,
        // atan(_hmd->DefaultEyeFov[0].LeftTan + _hmd->DefaultEyeFov[0].RightTan),
        _zNear,
        _zFar
        );
    _leftCameraNode = scene::Node::create("oculusLeftEye")
        ->addComponent(Transform::create())
        ->addComponent(leftCamera)
        ->addComponent(_leftRenderer);
    target->addChild(_leftCameraNode);

    auto rightCamera = PerspectiveCamera::create(
        _aspectRatio,
        0.78f,
        //atan(_hmd->DefaultEyeFov[1].LeftTan + _hmd->DefaultEyeFov[1].RightTan),
        _zNear,
        _zFar
        );
    _rightCameraNode = scene::Node::create("oculusRightEye")
        ->addComponent(Transform::create())
        ->addComponent(rightCamera)
        ->addComponent(_rightRenderer);
    target->addChild(_rightCameraNode);
}

void
WebVROculus::destroy()
{
}

void
WebVROculus::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
    //_assetLibrary = assetLibrary;
}

std::array<std::shared_ptr<geometry::Geometry>, 2>
WebVROculus::createDistortionGeometry(std::shared_ptr<render::AbstractContext> context)
{
    auto geometries = std::array<std::shared_ptr<geometry::Geometry>, 2>();

    /*
    auto options = Options::create()
        ->registerParser<SceneParser>("scene");

    auto loader = Loader::create(options)
        ->queue("model/vrGeometry.scene");

    loader->load();

    auto symbol = _assetLibrary->symbol("model/vrGeometry.scene");

    auto leftEyeGeometry = symbol->children().at(0)->component<Surface>()->geometry();
    auto rightEyeGeometry = symbol->children().at(1)->component<Surface>()->geometry();

    geometries[0] = leftEyeGeometry;
    geometries[1] = rightEyeGeometry;
    */

    return geometries;
}

EyeFOV
WebVROculus::getDefaultLeftEyeFov()
{
    EyeFOV leftEyeFov;

    leftEyeFov.DownTan = 0.f;
    leftEyeFov.LeftTan = 0.f;
    leftEyeFov.RightTan = 0.f;
    leftEyeFov.UpTan = 0.f;

    return leftEyeFov;
}

EyeFOV
WebVROculus::getDefaultRightEyeFov()
{
    EyeFOV rightEyeFov;

    rightEyeFov.DownTan = 0.f;
    rightEyeFov.LeftTan = 0.f;
    rightEyeFov.RightTan = 0.f;
    rightEyeFov.UpTan = 0.f;

    return rightEyeFov;
}

void
WebVROculus::updateCameraOrientation()
{
}