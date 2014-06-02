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
#include "minko/data/provider.hpp"
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

std::shared_ptr<Renderer> Hologram::_frontFaceNormalRenderer = nullptr;
std::shared_ptr<Renderer> Hologram::_backFaceNormalRenderer = nullptr;
std::shared_ptr<Renderer> Hologram::_depthMapRenderer = nullptr;

std::shared_ptr<render::Texture> Hologram::_frontFaceNormalRenderTarget = nullptr;
std::shared_ptr<render::Texture> Hologram::_backFaceNormalRenderTarget = nullptr;
std::shared_ptr<render::Texture> Hologram::_depthMapRenderTarget = nullptr;

const float Hologram::MAP_RESOLUTION = 1024.f;

Hologram::Hologram(std::shared_ptr<render::Effect>			frontEffect, 
				   std::shared_ptr<render::Effect>			backEffect,
				   std::shared_ptr<render::Effect>			depthEffect,
				   std::shared_ptr<render::AbstractContext> context) :
	_context(context),
	_depthEffect(depthEffect),
	_frontEffect(frontEffect),
	_backEffect(backEffect)
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
	if (_frontFaceNormalRenderer == nullptr)
	{
		_depthMapRenderTarget = render::Texture::create(_context, MAP_RESOLUTION, MAP_RESOLUTION, false, true);
		_depthMapRenderer = Renderer::create(0xFFFFFFFF, _depthMapRenderTarget, _depthEffect, 10.f);

		_depthMapRenderer->backgroundColor(0xFFFFFFFF);

		_frontFaceNormalRenderTarget = render::Texture::create(_context, MAP_RESOLUTION, MAP_RESOLUTION, false, true);
		_frontFaceNormalRenderer = Renderer::create(0x000000FF, _frontFaceNormalRenderTarget, _frontEffect, 10.f);

		_backFaceNormalRenderTarget = render::Texture::create(_context, MAP_RESOLUTION, MAP_RESOLUTION, false, true);
		_backFaceNormalRenderer = Renderer::create(0x000000FF, _backFaceNormalRenderTarget, _backEffect, 10.f);

		_frontFaceNormalRenderer->layoutMask(1u << 21);
		_backFaceNormalRenderer->layoutMask(1u << 21);
		_depthMapRenderer->layoutMask(1u << 21);

		auto nodeSet = scene::NodeSet::create(target->root())->descendants(false, false)->where(
			[=](scene::Node::Ptr node) -> bool
		{
			return node->hasComponent<PerspectiveCamera>();
		});

		nodeSet->nodes()[0]->addComponent(_frontFaceNormalRenderer);
		nodeSet->nodes()[0]->addComponent(_backFaceNormalRenderer);
		nodeSet->nodes()[0]->addComponent(_depthMapRenderer);
	}

	if (target->hasComponent<Surface>())
	{
		target->component<Surface>()->material()
			->set("frontFaceNormalMap", _frontFaceNormalRenderTarget)
			->set("backFaceNormalMap", _backFaceNormalRenderTarget)
			->set("depthMap", _depthMapRenderTarget)
			->set("mapResolution", MAP_RESOLUTION);

		target->layouts(target->layouts() | 1u << 21);
	}
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