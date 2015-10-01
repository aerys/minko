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

#include "minko/component/AbstractScript.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;

void
AbstractScript::targetAdded(Node::Ptr target)
{
	_componentAddedSlot = target->componentAdded().connect(
		[=](Node::Ptr n, Node::Ptr t, AbstractComponent::Ptr c)
		{
			componentAddedHandler(n, t, c);
		}
	);

	_componentRemovedSlot = target->componentRemoved().connect(
		[=](Node::Ptr n, Node::Ptr t, AbstractComponent::Ptr c)
		{
			componentRemovedHandler(n, t, c);
		}
	);

    auto addedOrRemovedCallback = [=](Node::Ptr n, Node::Ptr t, Node::Ptr p)
	{
		addedOrRemovedHandler(n, t, p);
	};
    _addedSlot = target->added().connect(addedOrRemovedCallback);
    _removedSlot = target->removed().connect(addedOrRemovedCallback);

	_started = false;

    if (target->root()->hasComponent<SceneManager>())
        setSceneManager(target->root()->component<SceneManager>());
}

void
AbstractScript::addedOrRemovedHandler(Node::Ptr node,
									  Node::Ptr target,
									  Node::Ptr parent)
{
	if (node->root() != target->root())
		return;

	setSceneManager(target->root()->component<SceneManager>());
}

void
AbstractScript::targetRemoved(Node::Ptr target)
{
	_componentAddedSlot     = nullptr;
	_componentRemovedSlot   = nullptr;
    _frameBeginSlot         = nullptr;
	_frameEndSlot           = nullptr;

    if (_started)
    {
        _started = false;
        stop(target);
    }
}

void
AbstractScript::componentAddedHandler(Node::Ptr					node,
									  Node::Ptr					target,
									  AbstractComponent::Ptr	component)
{
	SceneManager::Ptr sceneManager = std::dynamic_pointer_cast<SceneManager>(component);

	if (sceneManager)
		setSceneManager(sceneManager);
}

void
AbstractScript::componentRemovedHandler(Node::Ptr				node,
										Node::Ptr				target,
										AbstractComponent::Ptr	component)
{
	SceneManager::Ptr sceneManager = std::dynamic_pointer_cast<SceneManager>(component);

	if (sceneManager)
		setSceneManager(nullptr);
}

void
AbstractScript::frameBeginHandler(SceneManager::Ptr sceneManager,
								  float 			time,
								  float 			deltaTime)
{
    auto target = this->target();

    _time = time;
    _deltaTime = deltaTime;

    if (!_started && ready() && target != nullptr)
	{
        _started = true;
        start(target);
	}

    if (_started)
        update(target);

	if (!_started)
        stop(target);
}

void
AbstractScript::frameEndHandler(SceneManager::Ptr 	sceneManager,
								float 				time,
								float 				deltaTime)
{
	if (_started)
		end(target());
}

void
AbstractScript::setSceneManager(SceneManager::Ptr sceneManager)
{
	if (sceneManager && _enabled)
	{
        if (!_frameBeginSlot)
            _frameBeginSlot = sceneManager->frameBegin()->connect(
				[=](SceneManager::Ptr s, float t, float dt)
				{
					frameBeginHandler(s, t, dt);
				},
				priority()
			);
		if (!_frameEndSlot)
			_frameEndSlot = sceneManager->frameEnd()->connect(
				[=](SceneManager::Ptr s, float t, float dt)
				{
					frameEndHandler(s, t, dt);
				},
				priority()
			);
	}
	else if (_frameBeginSlot)
	{
        if (_started)
        {
		    _started = false;
		    stop(target());
        }

		_frameBeginSlot = nullptr;
		_frameEndSlot   = nullptr;
	}
}

void
AbstractScript::enabled(bool v)
{
	if (v != _enabled)
	{
		_enabled = v;
		_started = !v;

		if (target())
			setSceneManager(target()->root()->component<SceneManager>());
	}
}
