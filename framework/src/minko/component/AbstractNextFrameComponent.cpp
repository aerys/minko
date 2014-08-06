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

#include "AbstractNextFrameComponent.hpp"

#include "minko/component/SceneManager.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;

void
AbstractNextFrameComponent::initialize()
{
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &AbstractNextFrameComponent::targetAddedHandler,
        std::static_pointer_cast<AbstractNextFrameComponent>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _targetRemovedSlot = targetRemoved()->connect(std::bind(
        &AbstractNextFrameComponent::targetRemovedHandler,
        std::static_pointer_cast<AbstractNextFrameComponent>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));
}

void
AbstractNextFrameComponent::targetAddedHandler(AbstractComponent::Ptr cmp, NodePtr target)
{
    _componentAddedSlot = target->componentAdded()->connect(std::bind(
        &AbstractNextFrameComponent::componentAddedHandler,
        std::static_pointer_cast<AbstractNextFrameComponent>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _componentRemovedSlot = target->componentRemoved()->connect(std::bind(
        &AbstractNextFrameComponent::componentRemovedHandler,
        std::static_pointer_cast<AbstractNextFrameComponent>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
}

void
AbstractNextFrameComponent::targetRemovedHandler(AbstractComponent::Ptr cmp, NodePtr target)
{
    _componentAddedSlot = nullptr;
    _componentRemovedSlot = nullptr;
}

void
AbstractNextFrameComponent::componentAddedHandler(NodePtr node, NodePtr target, AbstractComponent::Ptr cmp)
{
    SceneManager::Ptr sceneManager = std::dynamic_pointer_cast<SceneManager>(cmp);

    if (sceneManager)
        setSceneManager(sceneManager);
}

void
AbstractNextFrameComponent::componentRemovedHandler(NodePtr node, NodePtr target, AbstractComponent::Ptr cmp)
{
    SceneManager::Ptr sceneManager = std::dynamic_pointer_cast<SceneManager>(cmp);

    if (sceneManager)
        setSceneManager(nullptr);
}

void
AbstractNextFrameComponent::setSceneManager(SceneManager::Ptr sceneManager)
{
    if (sceneManager)
    {

    }
    else
    {

    }
}
