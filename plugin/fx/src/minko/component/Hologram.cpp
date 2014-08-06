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

#include "minko/component/Hologram.hpp"
#include "minko/scene/Node.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/data/Container.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/Surface.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace math;
using namespace minko::component;

std::shared_ptr<Renderer> Hologram::_frontFaceNormalRenderer0 = nullptr;
std::shared_ptr<Renderer> Hologram::_backFaceNormalRenderer0 = nullptr;
std::shared_ptr<Renderer> Hologram::_depthMapRenderer0 = nullptr;

std::shared_ptr<Renderer> Hologram::_frontFaceNormalRenderer1 = nullptr;
std::shared_ptr<Renderer> Hologram::_backFaceNormalRenderer1 = nullptr;
std::shared_ptr<Renderer> Hologram::_depthMapRenderer1 = nullptr;

std::shared_ptr<render::Texture> Hologram::_frontFaceNormalRenderTarget0 = nullptr;
std::shared_ptr<render::Texture> Hologram::_backFaceNormalRenderTarget0 = nullptr;
std::shared_ptr<render::Texture> Hologram::_depthMapRenderTarget0 = nullptr;

std::shared_ptr<render::Texture> Hologram::_frontFaceNormalRenderTarget1 = nullptr;
std::shared_ptr<render::Texture> Hologram::_backFaceNormalRenderTarget1 = nullptr;
std::shared_ptr<render::Texture> Hologram::_depthMapRenderTarget1 = nullptr;

const uint Hologram::MAP_RESOLUTION = 1024;

Hologram::Hologram(std::shared_ptr<render::Effect>            effect,
                   std::shared_ptr<render::AbstractContext> context) :
    _context(context),
    _effect(effect)
{
}

void
Hologram::initialize()
{

    _targetAddedSlot = targetAdded()->connect([&](AbstractComponent::Ptr cmp, NodePtr target)
    {
        if (target->root()->hasComponent<SceneManager>())
            initTarget(nullptr, target, nullptr);
        else
            _addedToSceneSlot = target->added()->connect(std::bind(
            &Hologram::targetAddedToScene,
            std::static_pointer_cast<Hologram>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
    });
}

void
Hologram::initTarget(AbstractComponent::Ptr cmp, NodePtr target, NodePtr ancestor)
{
    if (_frontFaceNormalRenderer0 == nullptr && _frontFaceNormalRenderer1 == nullptr)
    {
        _depthMapRenderer0 = Renderer::create(0xFFFFFFFF, nullptr, _effect, 10.f);
        //_depthMapRenderer0->layoutMask(1u << 21);

        auto nodeSet = scene::NodeSet::create(target->root())->descendants(false, false)->where(
            [=](scene::Node::Ptr node) -> bool
        {
            return node->hasComponent<PerspectiveCamera>();
        });

        auto leftEyeNode = nodeSet->nodes()[0];

        //leftEyeNode->addComponent(_depthMapRenderer0);

        if (nodeSet->nodes().size() > 1)
        {
            auto rightEyeNode = nodeSet->nodes()[1];

            _depthMapRenderer1 = Renderer::create(0xFFFFFFFF, nullptr, _effect, 11.f);
            //_depthMapRenderer1->layoutMask(1u << 21);

            //rightEyeNode->addComponent(_depthMapRenderer1);
        }


    }

    if (target->hasComponent<Surface>())
    {

        /*target->component<Surface>()->material()
            ->set("mapResolution", float(MAP_RESOLUTION));*/

        /*target->component<Surface>()->material()
            ->set("frontFaceNormalMap", _frontFaceNormalRenderTarget1)
            ->set("backFaceNormalMap", _backFaceNormalRenderTarget1)
            ->set("depthMap", _depthMapRenderTarget1)
            ->set("mapResolution", float(MAP_RESOLUTION));*/

        //target->layouts(target->layouts() | 1u << 21);
    }

    //targetAddedToScene(target, target, target->parent());
}

void
Hologram::targetAddedHandler(AbstractComponent::Ptr cmp, NodePtr target)
{
    _addedToSceneSlot = target->added()->connect(std::bind(
        &Hologram::initTarget,
        std::static_pointer_cast<Hologram>(shared_from_this()),
        cmp,
        std::placeholders::_2,
        std::placeholders::_3
        ));
}

void
Hologram::targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor)
{
    _addedToSceneSlot = nullptr;

    for (auto target : targets())
        initTarget(shared_from_this(), target, ancestor);

    _frameBeginSlot = target->root()->component<SceneManager>()->frameBegin()->connect(
        [&](std::shared_ptr<SceneManager> sceneManager, float time, float deltaTime)
    {
        for (auto target : targets())
            target->component<Surface>()->material()->set("time", time);
    });
}