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

#include "minko/vr/WebVR.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Camera.hpp"
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
using namespace minko::vr;

WebVR::WebVR(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _zNear(zNear),
    _zFar(zFar)
{
    std::string eval;

    // Retrieve VRDisplay device and store it into window.MinkoVR object
    eval += "window.MinkoVR = {};                                                                       \n";
    eval += "function vrDeviceCallback(vrDisplays) {                                                    \n";
    eval += "    for (var i = 0; i < vrDisplays.length; ++i) {                                          \n";
    eval += "        var vrDisplay = vrDisplays[i];                                                     \n";
    eval += "        window.MinkoVR.vrDisplay = vrDisplay;                                              \n";
    eval += "        if (typeof(VRFrameData) != 'undefined')                                            \n";
    eval += "            window.MinkoVR.vrFrameData = new VRFrameData();                                \n";
    eval += "        break;                                                                             \n";
    eval += "    }                                                                                      \n";
    eval += "}                                                                                          \n";
    eval += "                                                                                           \n";
    eval += "                                                                                           \n";
    eval += "if (navigator.getVRDisplays !== undefined) {                                               \n";
    eval += "    navigator.getVRDisplays().then(vrDeviceCallback);                                      \n";
    eval += "}                                                                                          \n";

    // Define VRDisplay callbacks
    eval += "window.MinkoVR.onVRRequestPresent = function() {                                           \n";
    eval += "   console.log('onVRRequestPresent');                                                      \n";
    eval += "   var renderCanvas = document.getElementById('canvas');                                   \n";
    eval += "   window.MinkoVR.vrDisplay.requestPresent([{ source: renderCanvas }]).then(               \n";
    eval += "        function () {                                                                      \n";
    eval += "           console.log('Success: requestPresent succeed.')                                 \n";
    eval += "        },                                                                                 \n";
    eval += "        function () {                                                                      \n";
    eval += "           console.log('Error: requestPresent failed.');                                   \n";
    eval += "        }                                                                                  \n";
    eval += "   );                                                                                      \n";
    eval += " };                                                                                        \n";
    eval += "                                                                                           \n";

    eval += "window.MinkoVR.onVRExitPresent = function() {                                              \n";
    eval += "   console.log('onVRExitPresent');                                                         \n";
    eval += "    if (!window.MinkoVR.vrDisplay || !window.MinkoVR.vrDisplay.isPresenting)               \n";
    eval += "        return;                                                                            \n";
    eval += "                                                                                           \n";
    eval += "    window.MinkoVR.vrDisplay.exitPresent().then(                                           \n";
    eval += "        function () {                                                                      \n";
    eval += "            console.log('Success: exitPresent succeed.')                                   \n";
    eval += "        },                                                                                 \n";
    eval += "        function () {                                                                      \n";
    eval += "            console.log('Error: exitPresent failed.');                                     \n";
    eval += "        }                                                                                  \n";
    eval += "    );                                                                                     \n";
    eval += "}                                                                                          \n";

    eval += "function onVRPresentChange () {                                                            \n";
    eval += "    onResize();                                                                            \n";
    eval += "}                                                                                          \n";

    eval += "window.MinkoVR.onResize = function() {                                                    \n";
    eval += "   console.log('onResize');                                                                \n";
    // eval += "   var renderCanvas = document.getElementById('canvas');                                   \n";
    // eval += "   if (window.MinkoVR.vrDisplay && window.MinkoVR.vrDisplay.isPresenting) {                \n";
    // eval += "       var leftEye = window.MinkoVR.vrDisplay.getEyeParameters('left');                    \n";
    // eval += "       var rightEye = window.MinkoVR.vrDisplay.getEyeParameters('right');                  \n";
    // eval += "       renderCanvas.width = Math.max(leftEye.renderWidth, rightEye.renderWidth) * 2;       \n";
    // eval += "       renderCanvas.height = Math.max(leftEye.renderHeight, rightEye.renderHeight);        \n";
    // eval += "   } else {                                                                                \n";
    // eval += "       renderCanvas.width = renderCanvas.offsetWidth * window.devicePixelRatio;            \n";
    // eval += "       renderCanvas.height = renderCanvas.offsetHeight * window.devicePixelRatio;          \n";
    // eval += "   }                                                                                       \n";
    eval += "}                                                                                          \n";

    emscripten_run_script(eval.c_str());
}

void
WebVR::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
}

void
WebVR::initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window)
{
    _leftRenderer = leftRenderer;
    _rightRenderer = rightRenderer;
}

void
WebVR::targetAdded()
{
    _renderingEndSlot = _rightRenderer->renderingEnd()->connect([&](std::shared_ptr<minko::component::Renderer> rightRenderer)
    {
        std::string eval;
        eval += "if (!!window.MinkoVR.vrDisplay && window.MinkoVR.vrDisplay.isPresenting) {             \n";
        // Workaround To avoid a crash in Chrome VR
        eval += "    if (!!window.MinkoVR.vrFrameData && !!window.MinkoVR.vrFrameData.pose &&           \n";
        eval += "        !!window.MinkoVR.vrFrameData.pose.orientation) {                               \n";
        eval += "        window.MinkoVR.vrDisplay.submitFrame();                                        \n";
        eval += "    }                                                                                  \n";
        eval += "}                                                                                      \n";

        emscripten_run_script_int(eval.c_str());
    });

    std::string eval;
    eval += "window.addEventListener('vrdisplayactivate', window.MinkoVR.onVRRequestPresent, false);        \n";
    eval += "window.addEventListener('vrdisplaydeactivate', window.MinkoVR.onVRExitPresent, false);         \n";
    eval += "window.addEventListener('vrdisplaypresentchange', window.MinkoVR.onVRPresentChange, false);    \n";
    eval += "window.addEventListener('resize', window.MinkoVR.onResize, false);                             \n";

    emscripten_run_script(eval.c_str());
}

void
WebVR::targetRemoved()
{
    _renderingEndSlot = nullptr;

    std::string eval;
    eval += "window.removeEventListener('vrdisplayactivate', window.MinkoVR.onVRRequestPresent, false);        \n";
    eval += "window.removeEventListener('vrdisplaydeactivate', window.MinkoVR.onVRExitPresent, false);         \n";
    eval += "window.removeEventListener('vrdisplaypresentchange', window.MinkoVR.onVRPresentChange, false);    \n";
    eval += "window.removeEventListener('resize', window.MinkoVR.onResize, false);                             \n";

    emscripten_run_script(eval.c_str());
}

void
WebVR::updateViewport(int viewportWidth, int viewportHeight)
{
}

bool
WebVR::detected()
{
    auto eval = std::string("if (navigator.getVRDisplays != undefined) (1); else (0);");
    bool result = emscripten_run_script_int(eval.c_str()) != 0;

    return result;
}

float
WebVR::getLeftEyeFov()
{
    // FIXME: Should depend on the VRDisplay
    // vr Rift CV1
    // leftDegrees:43.97737166932644 (0.76754993200111731877)
    // rightDegrees:35.5747704995392 (0.6208968758567666724)
    // fov = atan(leftDegrees + rightDegrees) = 0.94
    return 0.94f;
}

float
WebVR::getRightEyeFov()
{
    // FIXME: Should depend on the VRDisplay
    // vr Rift CV1
    // leftDegrees:43.97737166932644 (0.76754993200111731877)
    // rightDegrees:35.5747704995392 (0.6208968758567666724)
    // fov = atan(leftDegrees + rightDegrees) = 0.94
    return 0.94f;
}

void
WebVR::updateCamera(scene::Node::Ptr target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    std::string eval;

    // Get VRDisplay orientation
    eval = "";
    eval += "if (!!window.MinkoVR.vrFrameData && !!window.MinkoVR.vrFrameData.pose &&               \n";
    eval += "    !!window.MinkoVR.vrFrameData.pose.orientation) {                                   \n";
    eval += "    window.MinkoVR.vrFrameData.pose.orientation.join(' ');                             \n";
    eval += "}                                                                                      \n";

    auto orientationString = std::string(emscripten_run_script_string(eval.c_str()));

    if (orientationString != "undefined")
    {
        std::array<float, 4> orientation;
        std::stringstream ssOrientation(orientationString);

        for (auto i = 0; i < 4; i++)
            ssOrientation >> orientation[i];

        auto quaternion = math::quat(orientation[3], orientation[0], orientation[1], orientation[2]);

        auto matrix = glm::mat4_cast(quaternion);
        target->component<Transform>()->matrix(matrix);
    }

    // Get VRDisplay position
    eval = "";
    eval += "if (!!window.MinkoVR.vrFrameData && !!window.MinkoVR.vrFrameData.pose &&               \n";
    eval += "    !!window.MinkoVR.vrFrameData.pose.position) {                                      \n";
    eval += "    window.MinkoVR.vrFrameData.pose.position.join(' ');                                \n";
    eval += "}                                                                                      \n";

    auto positionString = std::string(emscripten_run_script_string(eval.c_str()));

    if (positionString != "undefined")
    {
        std::array<float, 3> position;
        std::stringstream ssPosition(positionString);

        ssPosition >> position[0];
        ssPosition >> position[1];
        ssPosition >> position[2];

        auto hmdPosition = math::vec3(position[0], position[1], position[2]);
        auto translation = math::translate(hmdPosition);

        target->component<Transform>()->matrix(translation * target->component<Transform>()->matrix());
    }

    // Get VRDisplay projection matrixes
    eval = "";
    eval += "if (!!window.MinkoVR.vrDisplay && !!window.MinkoVR.vrFrameData) {                      \n";
    eval += "   window.MinkoVR.vrDisplay.getFrameData(window.MinkoVR.vrFrameData);                  \n";
    eval += "   if (!!window.MinkoVR.vrFrameData.leftProjectionMatrix &&                            \n";
    eval += "       !!window.MinkoVR.vrFrameData.rightProjectionMatrix) {                           \n";
    eval += "       window.MinkoVR.vrFrameData.leftProjectionMatrix.join(' ') + ' ' +               \n";
    eval += "       window.MinkoVR.vrFrameData.rightProjectionMatrix.join(' ');                     \n";
    eval += "   }                                                                                   \n";
    eval += "}                                                                                      \n";

    auto projectionMatrixesString = std::string(emscripten_run_script_string(eval.c_str()));

    if (projectionMatrixesString != "undefined")
    {
        std::array<float, 16> leftProjectionMatrix;
        std::array<float, 16> rightProjectionMatrix;
        std::stringstream ssProjectionMatrixes(projectionMatrixesString);

        for (auto i = 0; i < 16; i++)
            ssProjectionMatrixes >> leftProjectionMatrix[i];

        for (auto i = 0; i < 16; i++)
            ssProjectionMatrixes >> rightProjectionMatrix[i];

        auto leftMatrix = glm::make_mat4(leftProjectionMatrix.data());
        auto rightMatrix = glm::make_mat4(rightProjectionMatrix.data());

        _leftRenderer->target()->component<Camera>()->projectionMatrix(leftMatrix);
        _rightRenderer->target()->component<Camera>()->projectionMatrix(rightMatrix);
    }
}