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
#include "minko/component/Surface.hpp"
#include "minko/component/VideoCameraPreview.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/material/BasicMaterial.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/RectangleTexture.hpp"
#include "minko/scene/Node.hpp"
#include "minko/video/AbstractVideoCamera.hpp"
#include "minko/video/ImageFormat.hpp"

#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::scene;
using namespace minko::video;

VideoCameraPreview::VideoCameraPreview() :
    AbstractComponent()
{
}

void
VideoCameraPreview::initialize()
{
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &VideoCameraPreview::targetAddedHandler,
        std::static_pointer_cast<VideoCameraPreview>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _targetRemovedSlot = targetRemoved()->connect(std::bind(
        &VideoCameraPreview::targetRemovedHandler,
        std::static_pointer_cast<VideoCameraPreview>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _frameBeginSlot = _sceneManager->frameBegin()->connect(std::bind(
        &VideoCameraPreview::frameBeginHandler,
        std::static_pointer_cast<VideoCameraPreview>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _frameEndSlot = _sceneManager->frameEnd()->connect(std::bind(
        &VideoCameraPreview::frameEndHandler,
        std::static_pointer_cast<VideoCameraPreview>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _previewSurface = Surface::create(
        QuadGeometry::create(_context, 1, 1, 1, 1),
        material::BasicMaterial::create(),
        _sceneManager->assets()->effect("effect/Basic.effect")
    );
}

void
VideoCameraPreview::targetAddedHandler(AbstractComponent::Ptr component, scene::Node::Ptr target)
{
    _videoCameraFrameReceivedSlot = _videoCamera->frameReceived()->connect(
        std::bind(
            &VideoCameraPreview::frameReceivedHandler,
            std::static_pointer_cast<VideoCameraPreview>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            std::placeholders::_5
    ));

    if (_videoPreviewTarget != nullptr)
        _previewSurface->effect()->setUniform("uDiffuseMap", _videoPreviewTarget);

    target->addComponent(_previewSurface);
}

void
VideoCameraPreview::targetRemovedHandler(AbstractComponent::Ptr component, scene::Node::Ptr target)
{
    target->removeComponent(_previewSurface);

    _previewSurface = nullptr;

    _videoCameraFrameReceivedSlot = nullptr;
}

void
VideoCameraPreview::frameBeginHandler(SceneManager::Ptr sceneManager, float time, float deltaTime)
{
}

void
VideoCameraPreview::frameEndHandler(SceneManager::Ptr sceneManager, float time, float deltaTime)
{
}

void
VideoCameraPreview::frameReceivedHandler(AbstractVideoCamera::Ptr           videoCamera,
                                         const std::vector<unsigned char>&  data,
                                         int                                width,
                                         int                                height,
                                         ImageFormatType                    format)
{
    if (_videoPreviewTarget == nullptr ||
        width != _videoPreviewTarget->width() ||
        height != _videoPreviewTarget->height())
    {
        initializeVideoPreviewTarget(width, height, format);
    }
    else
    {
        updateVideoPreviewTarget(data, width, height, format);
    }
}

void
VideoCameraPreview::forceBackgroundUpdate()
{
    if (_previewSurface != nullptr && _videoPreviewTarget != nullptr)
    {
        _previewSurface->effect()->setUniform("uDiffuseMap", _videoPreviewTarget);
    }
}

void
VideoCameraPreview::initializeVideoPreviewTarget(int width, int height, ImageFormatType format)
{
    if (_videoPreviewTarget != nullptr)
        _videoPreviewTarget->dispose();

    switch (format)
    {
    case ImageFormatType::RGB:
        _videoPreviewTarget = RectangleTexture::create(_context, width, height, render::TextureFormat::RGB);
        break;
    case ImageFormatType::RGBA:
        _videoPreviewTarget = RectangleTexture::create(_context, width, height, render::TextureFormat::RGBA);
        break;
    }

    auto data = std::vector<unsigned char>(width * height * 4, 0);

    updateVideoPreviewTarget(data, width, height, format);

    if (_previewSurface != nullptr)
    {
        _previewSurface->effect()->setUniform("uDiffuseMap", _videoPreviewTarget);
        _previewSurface->effect()->setUniform("cameraRatio", float(width) / float(height));
    }
}

void
VideoCameraPreview::updateVideoPreviewTarget(const std::vector<unsigned char>&  data,
                                             int                                width,
                                             int                                height,
                                             ImageFormatType                    format)
{
    // TODO
    // setup specific (hardware) conversion for formats such as YUV

    _videoPreviewTarget->data(const_cast<unsigned char*>(data.data()), width, height);
}
