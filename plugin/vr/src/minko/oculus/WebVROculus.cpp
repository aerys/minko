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

#include "minko/oculus/WebVROculus.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"

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
    _zNear(zNear),
    _zFar(zFar),
    _initialized(false)
{
}

void
WebVROculus::initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window)
{
    std::string eval = "";

    eval += "function vrDeviceCallback(vrdevs) {                             \n";
    eval += "    for (var i = 0; i < vrdevs.length; ++i) {                   \n";
    eval += "        if (vrdevs[i] instanceof HMDVRDevice) {                 \n";
    eval += "            vrHMD = vrdevs[i];                                  \n";
    eval += "            break;                                              \n";
    eval += "        }                                                       \n";
    eval += "    }                                                           \n";
    eval += "    for (var i = 0; i < vrdevs.length; ++i) {                   \n";
    eval += "        if (vrdevs[i] instanceof PositionSensorVRDevice &&      \n";
    eval += "            vrdevs[i].hardwareUnitId == vrHMD.hardwareUnitId) { \n";
    eval += "            window.vrHMDSensor = vrdevs[i];                     \n";
    eval += "            break;                                              \n";
    eval += "        }                                                       \n";
    eval += "    }                                                           \n";
    eval += "}                                                               \n";
    eval += "                                                                \n";
    eval += "window.addEventListener(\"keydown\", function(e) {              \n";
    eval += "    if (e.keyCode == 70) {                                      \n";
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
    eval += "    }                                                           \n";
    eval += "}, false);                                                      \n";
    eval += "                                                                \n";
    eval += "if (navigator.getVRDevices !== undefined) {                     \n";
    eval += "    navigator.getVRDevices().then(vrDeviceCallback);            \n";
    eval += "}                                                               \n";


    emscripten_run_script(eval.c_str());
}

void
WebVROculus::targetRemoved()
{
}

void
WebVROculus::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
    std::string eval = "";

    eval += "var renderCanvas = document.getElementById('canvas');   \n";
    eval += "if (renderCanvas.mozRequestFullScreen) {                \n";
    eval += "    renderCanvas.mozRequestFullScreen({                 \n";
    eval += "        vrDisplay: vrHMD                                \n";
    eval += "    });                                                 \n";
    eval += "} else if (renderCanvas.webkitRequestFullscreen) {      \n";
    eval += "    renderCanvas.webkitRequestFullscreen({              \n";
    eval += "        vrDisplay: vrHMD,                               \n";
    eval += "    });                                                 \n";
    eval += "}                                                       \n";

    //emscripten_run_script(eval.c_str());
}

void
WebVROculus::updateViewport(int viewportWidth, int viewportHeight)
{
}

bool
WebVROculus::detected()
{
    auto eval = std::string("if (navigator.getVRDevices != undefined || navigator.mozGetVRDevices != undefined) (1); else (0);");
    bool result = emscripten_run_script_int(eval.c_str()) != 0;

    return result;
}

float
WebVROculus::getLeftEyeFov()
{
    return 1.91f;
}

float
WebVROculus::getRightEyeFov()
{
    return 1.91f;
}

void
WebVROculus::updateCameraOrientation(scene::Node::Ptr target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    if (!_initialized)
    {
        auto checkVrHDM = std::string("window.vrHMDSensor != null ? 1 : 0;");
        auto result = emscripten_run_script_int(checkVrHDM.c_str());

        if (result == 0)
            return;
        else
            _initialized = true;
    }

    std::string eval = "if (window.vrHMDSensor.getState().orientation != null) { window.vrHMDSensor.getState().orientation.x + ' ' + window.vrHMDSensor.getState().orientation.y + ' ' + window.vrHMDSensor.getState().orientation.z + ' ' + window.vrHMDSensor.getState().orientation.w; }\n";
    auto s = std::string(emscripten_run_script_string(eval.c_str()));

    if (s != "undefined")
    {
        std::array<float, 4> orientation;
        std::stringstream ssOrientation(s);

        ssOrientation >> orientation[0];
        ssOrientation >> orientation[1];
        ssOrientation >> orientation[2];
        ssOrientation >> orientation[3];

    	auto quaternion = math::quat(orientation[3], orientation[0], orientation[1], orientation[2]);

    	auto matrix = glm::mat4_cast(quaternion);
    	target->component<Transform>()->matrix(matrix);
    }

    // Get position tracking
	/*
    eval = "if (window.vrHMDSensor.getState().position != null) { window.vrHMDSensor.getState().position.x + ' ' + window.vrHMDSensor.getState().position.y + ' ' + window.vrHMDSensor.getState().position.z; }\n";
    s = std::string(emscripten_run_script_string(eval.c_str()));

    if (s != "undefined")
    {
        std::array<float, 3> position;
        std::stringstream ssPosition(s);

        ssPosition >> position[0];
        ssPosition >> position[1];
        ssPosition >> position[2];

    	target->component<Transform>()->matrix(math::translate(math::vec3(position[0], position[1], position[2])) * target->component<Transform>()->matrix());
	}
	*/
}
