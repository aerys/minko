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

#include "minko/oculus/Cardboard.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Texture.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::oculus;
using namespace minko::render;
using namespace minko::file;

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
using namespace minko::sensors;
#endif

Cardboard::Cardboard(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _zNear(zNear),
    _zFar(zFar),
    _magnetChangedSlot(nullptr),
    _magnetSensorData(),
    _magnetPressed(Signal<>::create()),
    _magnetReleased(Signal<>::create()),
    _isMagnetDown(false),
    _magnetOffset()
{
    _uvScaleOffset[0].first = math::vec2();
    _uvScaleOffset[0].second = math::vec2();
    _uvScaleOffset[1].first = math::vec2();
    _uvScaleOffset[1].second = math::vec2();
}

void
Cardboard::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
}

void
Cardboard::initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _attitude = Attitude::getInstance();
    _magnetometer = Magnetometer::getInstance();

    _attitude->initialize();
    _magnetometer->initialize();

    _attitude->startTracking();
    _magnetometer->startTracking();

    _magnetChangedSlot = _magnetometer->onSensorChanged()->connect([&](float x, float y, float z)
    {
        if (_magnetSensorData.size() > 40)
            _magnetSensorData.erase(_magnetSensorData.begin());

        float value[3] = {x, y, z};

        _magnetSensorData.push_back(std::vector<float>(value, value + 3));

        evaluateMagnetModel();
    });
#endif
}

void
Cardboard::updateViewport(int viewportWidth, int viewportHeight)
{
}

void
Cardboard::targetRemoved()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _attitude->stopTracking();
    _magnetometer->stopTracking();
#endif
}

float
Cardboard::getLeftEyeFov()
{
    return atan(45);
}

float
Cardboard::getRightEyeFov()
{
    return atan(45);
}

void
Cardboard::updateCameraOrientation(std::shared_ptr<scene::Node> target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    auto rotationMatrix = math::transpose(_attitude->rotationMatrix());

    target->component<Transform>()->matrix(rotationMatrix);
#endif
}

void
Cardboard::evaluateMagnetModel()
{
    if (_magnetSensorData.size() < 40)
        return;

    float means[2] = {};
    float maximums[2] = {};
    float minimums[2] = {};

    for (int i = 0; i < 2; i++) {
        int segmentStart = 20 * i;

        std::vector<float> offsets = computeMagnetOffsets(segmentStart, _magnetSensorData[_magnetSensorData.size() - 1]);

        means[i] = computeMean(offsets);
        maximums[i] = computeMaximum(offsets);
        minimums[i] = computeMinimum(offsets);
    }

    //LOG_INFO(minimums[0] << " - " << maximums[1]);
    if ((minimums[0] < 30.0F) && (maximums[1] > 130.0F))
    {
        _magnetSensorData.clear();

        _isMagnetDown = !_isMagnetDown;

        if(_isMagnetDown)
            _magnetPressed->execute();
        else
            _magnetReleased->execute();
    }
}

std::vector<float>
Cardboard::computeMagnetOffsets(int start, std::vector<float> baseline)
{
    _magnetOffset.clear();
    _magnetOffset.assign(20, 0);
    for (int i = 0; i < 20; i++) {
        std::vector<float> point = _magnetSensorData[start + i];
        float o[] = { point[0] - baseline[0], point[1] - baseline[1], point[2] - baseline[2] };
        float magnitude = sqrt(o[0] * o[0] + o[1] * o[1] + o[2] * o[2]);
        _magnetOffset[i] = magnitude;
    }
    return _magnetOffset;
}

float
Cardboard::computeMean(std::vector<float> offsets) {
    float sum = 0.0f;

    for (auto i = 0; i < offsets.size(); i++)
        sum += offsets[i];

    return sum / offsets.size();
}

float
Cardboard::computeMaximum(std::vector<float> offsets) {
    float max = -1.0f;

    for (auto i = 0; i < offsets.size(); i++)
        max = std::max(offsets[i], max);

    return max;
}

float
Cardboard::computeMinimum(std::vector<float> offsets) {
    float min = 1.0f;

    for (auto i = 0; i < offsets.size(); i++)
        min = std::min(offsets[i], min);

    return min;
}