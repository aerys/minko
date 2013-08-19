/*
Copyright (c) 2013 Aerys

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

#include "AbstractLight.hpp"

#include "minko/scene/Node.hpp"
#include "minko/component/LightManager.hpp"

using namespace minko::component;

AbstractLight::AbstractLight(const std::string& arrayName) :
	AbstractComponent(),
	_arrayData(data::ArrayProvider::create(arrayName, 0)),
	_color(math::Vector3::create(1, 1, 1))
{
	data()->set("color", _color);
}

void
AbstractLight::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&AbstractLight::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&AbstractLight::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
AbstractLight::targetAddedHandler(AbsCmpPtr cmp, NodePtr target)
{
	if (!target->root()->hasComponent<LightManager>())
		target->root()->addComponent(LightManager::create());

	auto cb = std::bind(
		&AbstractLight::addedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	_addedSlots[target] = target->added()->connect(cb);
	_removedSlots[target] = target->removed()->connect(cb);
}

void
AbstractLight::targetRemovedHandler(AbsCmpPtr cmp, NodePtr target)
{
	_addedSlots.erase(target);
	_removedSlots.erase(target);
}

void
AbstractLight::addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	if (node != target)
		return;

	if (!node->root()->hasComponent<LightManager>())
		node->root()->addComponent(LightManager::create());
}
