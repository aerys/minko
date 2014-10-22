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
AbstractScript::initialize()
{
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &AbstractScript::targetAddedHandler,
        std::static_pointer_cast<AbstractScript>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _targetRemovedSlot = targetRemoved()->connect(std::bind(
        &AbstractScript::targetRemovedHandler,
        std::static_pointer_cast<AbstractScript>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));
}

void
AbstractScript::targetAddedHandler(AbstractComponent::Ptr cmp, scene::Node::Ptr target)
{
    _componentAddedSlot = target->componentAdded()->connect(std::bind(
        &AbstractScript::componentAddedHandler,
        std::static_pointer_cast<AbstractScript>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _componentRemovedSlot = target->componentRemoved()->connect(std::bind(
        &AbstractScript::componentRemovedHandler,
        std::static_pointer_cast<AbstractScript>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    auto addedOrRemovedCallback = std::bind(
        &AbstractScript::addedOrRemovedHandler,
        std::static_pointer_cast<AbstractScript>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );
    _addedSlot = target->added()->connect(addedOrRemovedCallback);
    _removedSlot = target->removed()->connect(addedOrRemovedCallback);

    _started[target] = false;

    if (target->root()->hasComponent<SceneManager>())
        setSceneManager(target->root()->component<SceneManager>());
}

void
AbstractScript::addedOrRemovedHandler(scene::Node::Ptr node, scene::Node::Ptr target, scene::Node::Ptr parent)
{
    if (node->root() != target->root())
        return;

    if (target->root()->hasComponent<SceneManager>())
        setSceneManager(target->root()->component<SceneManager>());
    else
        setSceneManager(nullptr);
}

void
AbstractScript::targetRemovedHandler(AbstractComponent::Ptr cmp, scene::Node::Ptr target)
{
    _started[target] = false;
    stop(target);
    
    _componentAddedSlot     = nullptr;
    _componentRemovedSlot   = nullptr;
    _frameBeginSlot         = nullptr;
    _frameEndSlot           = nullptr;

    if (running(target))
    {
        _started[target] = false;
        stop(target);
    }
}

void
AbstractScript::componentAddedHandler(scene::Node::Ptr            node,
                                      scene::Node::Ptr            target,
                                      AbstractComponent::Ptr    component)
{
    findSceneManager();

    //auto sceneManager = std::dynamic_pointer_cast<SceneManager>(component);

    //if (sceneManager)
    //    setSceneManager(sceneManager);
}

void
AbstractScript::componentRemovedHandler(scene::Node::Ptr        node,
                                        scene::Node::Ptr        target,
                                        AbstractComponent::Ptr    component)
{
    findSceneManager();

    //auto sceneManager = std::dynamic_pointer_cast<SceneManager>(component);

    //if (sceneManager)
    //    setSceneManager(nullptr);
}

void
AbstractScript::frameBeginHandler(SceneManager::Ptr sceneManager, float time, float deltaTime)
{
    _time = time;
    _deltaTime = deltaTime;

    for (auto& target : targets())
    {
        if (!_started[target] && ready(target))
        {
            _started[target] = true;

            start(target);
        }

        if (running(target))
            update(target);
        else
        {
            _started[target] = false;
            stop(target);
        }
    }
}

void
AbstractScript::frameEndHandler(std::shared_ptr<SceneManager> sceneManager, float time, float deltaTime)
{
    for (auto& target : targets())
    {
        if (running(target))
            end(target);
    }
}

void
AbstractScript::findSceneManager()
{
    NodeSet::Ptr roots = NodeSet::create(targets())
        ->roots()
        ->where([](NodePtr node)
        {
            return node->hasComponent<SceneManager>();
        });

    if (roots->nodes().size() > 1)
        throw std::logic_error("The same script cannot be in two separate scenes.");
    else if (roots->nodes().size() == 1)
        setSceneManager(roots->nodes()[0]->component<SceneManager>());
    else
        setSceneManager(nullptr);
}

void
AbstractScript::setSceneManager(SceneManager::Ptr sceneManager)
{
    if (sceneManager)
    {
        if (!_frameBeginSlot)
            _frameBeginSlot = sceneManager->frameBegin()->connect(std::bind(
                &AbstractScript::frameBeginHandler,
                std::static_pointer_cast<AbstractScript>(shared_from_this()),
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            ));
        if (!_frameEndSlot)
            _frameEndSlot = sceneManager->frameEnd()->connect(std::bind(
                &AbstractScript::frameEndHandler,
                std::static_pointer_cast<AbstractScript>(shared_from_this()),
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            ));
    }
    else if (_frameBeginSlot)
    {
        for (auto& target : targets())
        {
            _started[target] = false;
            stop(target);
        }

        _frameBeginSlot = nullptr;
        _frameEndSlot   = nullptr;
    }
}
