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
#include "minko/component/Camera.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/scene/Node.hpp"

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
}

void
WebVR::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
    LOG_INFO("Initialize WebVR");
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
        emscripten_run_script("window.MinkoVR.submitFrame();");
    });

    emscripten_run_script("window.MinkoVR.bindEvents();");
}

void
WebVR::targetRemoved()
{
    _renderingEndSlot = nullptr;

    emscripten_run_script("window.MinkoVR.unbindEvents();");
}

void
WebVR::updateViewport(int viewportWidth, int viewportHeight)
{
}

bool
WebVR::supported()
{
    auto eval = std::string("if (navigator.getVRDisplays != undefined) (1); else (0);");
    bool result = emscripten_run_script_int(eval.c_str()) != 0;

    return result;
}

bool
WebVR::detected()
{
    // TODO: Find a way to detect HMD
    // Wait until the promise to detect the HMD has been resolved <= doesn't work
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // auto detected = emscripten_run_script_string("!!window.MinkoVR && !!window.MinkoVR.ready;");
    // LOG_INFO("WebVR detected: " << detected);
    // return detected == std::string("true");

    // For now we just check that the device
    // is supported by the current browser
    return WebVR::supported();
}

float
WebVR::getLeftEyeFov()
{
    // FIXME: Should depend on the VRDisplay
    // Oculus Rift CV1
    // leftDegrees: 43.97737166932644° (0.76754993200111731877 rad)
    // rightDegrees: 35.5747704995392° (0.6208968758567666724 rad)
    // leftDegrees + rightDegrees = 79.5521421689° (1.388446807858484 rad)
    // upDegrees: 41.65303039550781 (0.72698252383308381575)
    // downDegrees: 48.00802230834961 (0.83789805664951055864)
    // upDegrees + downDegrees = 89.6610527039° (1.564880580483337 rad)
    // horizontal fov = atan(leftDegrees + rightDegrees) = 1.5582266170886 rad (89.279808684346051°)
    // vetical fov = atan(upDegrees + downDegrees) = 1.5596436745680 rad (89.36100009724332°)
    return 1.56f;
}

float
WebVR::getRightEyeFov()
{
    // FIXME: Should depend on the VRDisplay
    // Oculus Rift CV1
    // leftDegrees: 43.97737166932644° (0.76754993200111731877 rad)
    // rightDegrees: 35.5747704995392° (0.6208968758567666724 rad)
    // leftDegrees + rightDegrees = 79.5521421689° (1.388446807858484 rad)
    // upDegrees: 41.65303039550781 (0.72698252383308381575)
    // downDegrees: 48.00802230834961 (0.83789805664951055864)
    // upDegrees + downDegrees = 89.6610527039° (1.564880580483337 rad)
    // horizontal fov = atan(leftDegrees + rightDegrees) = 1.5582266170886 rad (89.279808684346051°)
    // vetical fov = atan(upDegrees + downDegrees) = 1.5596436745680 rad (89.36100009724332°)

    return 1.56f;
}

void
WebVR::updateCamera(scene::Node::Ptr target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    auto ready = emscripten_run_script_string("!!window.MinkoVR && !!window.MinkoVR.ready;") == std::string("true");

    if (!ready)
        return;

    // Get VRDisplay orientation
    auto orientationString = std::string(emscripten_run_script_string("window.MinkoVR.getOrientation();"));

    if (orientationString != "null")
    {
        std::array<float, 4> orientation;
        std::stringstream ssOrientation(orientationString);

        for (auto i = 0; i < 4; i++)
            ssOrientation >> orientation[i];

        auto quaternion = math::quat(orientation[3], orientation[0], orientation[1], orientation[2]);

        auto matrix = glm::mat4_cast(quaternion);
        target->component<Transform>()->matrix(matrix);
    }

    if (!_disablePositionTracking)
    {
        // Get VRDisplay position
        auto positionString = std::string(emscripten_run_script_string("window.MinkoVR.getPosition();"));

        if (positionString != "null")
        {
            std::array<float, 3> position;
            std::stringstream ssPosition(positionString);

            ssPosition >> position[0];
            ssPosition >> position[1];
            ssPosition >> position[2];

            // TODO: Position tracking scale should be set on VRCamera component
            auto scale = 1.f;
            auto hmdPosition = math::vec3(position[0], position[1], position[2]) * scale;
            auto translation = math::translate(hmdPosition);

            target->component<Transform>()->matrix(translation * target->component<Transform>()->matrix());
        }
    }

    // Get VRDisplay projection Matrices
    auto projectionMatricesString = std::string(emscripten_run_script_string("window.MinkoVR.getProjectionMatrices();"));

    if (projectionMatricesString != "null")
    {
        std::array<float, 16> leftProjectionMatrix;
        std::array<float, 16> rightProjectionMatrix;
        std::stringstream ssProjectionMatrices(projectionMatricesString);

        for (auto i = 0; i < 16; i++)
            ssProjectionMatrices >> leftProjectionMatrix[i];

        for (auto i = 0; i < 16; i++)
            ssProjectionMatrices >> rightProjectionMatrix[i];

        auto leftMatrix = glm::make_mat4(leftProjectionMatrix.data());
        auto rightMatrix = glm::make_mat4(rightProjectionMatrix.data());

        _leftRenderer->target()->component<Camera>()->projectionMatrix(leftMatrix);
        _rightRenderer->target()->component<Camera>()->projectionMatrix(rightMatrix);
    }
}
