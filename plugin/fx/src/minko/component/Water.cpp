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

#include "minko/component/Water.hpp"
#include "minko/scene/Node.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/data/provider.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/data/Container.hpp"
#include "minko/material/WaterMaterial.hpp"

using namespace minko;
using namespace math;
using namespace minko::component;

Water::Water(float cycle, std::shared_ptr<material::WaterMaterial> mat) :
_cycle(cycle),
_waterMaterial(mat)
{
	_provider = data::Provider::create();
}

void
Water::initialize()
{

	_targetAddedSlot = targetAdded()->connect([&](AbstractComponent::Ptr cmp, NodePtr target)
	{

		if (target->root()->hasComponent<SceneManager>())
			targetAddedToScene(nullptr, target, nullptr);
		else
			_addedToSceneSlot = target->added()->connect(std::bind(
			&Water::targetAddedToScene,
			std::static_pointer_cast<Water>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3));
	});
}

void
Water::targetAddedHandler(AbstractComponent::Ptr cmp, NodePtr target)
{
	target->added()->connect([&](NodePtr node, NodePtr target, NodePtr ancestor)
	{
		if (target->root()->hasComponent<SceneManager>())
			targetAddedToScene(nullptr, target, nullptr);
	});
}

void
Water::targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor)
{
	_addedToSceneSlot = nullptr;

	if (!target->root()->data()->hasProvider(_provider))
	{
		_provider->set<float>("offsetTime", 0);
		_provider->set<float>("frameId", 0);
		target->root()->data()->addProvider(_provider);
	}

	// Listen scene manager
	_frameBeginSlot = target->root()->component<SceneManager>()->frameBegin()->connect(
		[&](std::shared_ptr<SceneManager> sceneManager, float time, float deltaTime)
	{
		_provider->set<float>("frameId", _provider->get<float>("frameId") + 1);

		float flowMapOffset1 = _waterMaterial->flowMapOffset1();
		float flowMapOffset2 = _waterMaterial->flowMapOffset2();
		float flowSpeed = _waterMaterial->normalMapSpeed();

		flowMapOffset1 += flowSpeed * deltaTime;
		flowMapOffset2 += flowSpeed * deltaTime;

		if (flowMapOffset1 >= _cycle)
			flowMapOffset1 -= _cycle;

		if (flowMapOffset2 >= _cycle)
			flowMapOffset2 -= _cycle;

		_waterMaterial->flowMapOffset1(flowMapOffset1);
		_waterMaterial->flowMapOffset2(flowMapOffset2);
	});
}