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

#include "SceneManager.hpp"

#include "minko/scene/Node.hpp"

using namespace minko::component;

SceneManager::SceneManager() :
    _frameId(0),
    _enterFrame(Signal<Ptr>::create()),
    _exitFrame(Signal<Ptr>::create())
{
}

void
SceneManager::initialize()
{
    /*
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &SceneManager::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
    _targetRemovedSlot = targetAdded()->connect(std::bind(
        &SceneManager::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
    */
}

void
SceneManager::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
    /*
    _addedSlot = target->added()->connect(std::bind(
        &SceneManager::addedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
    */
}

void
SceneManager::targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
    _addedSlot = nullptr;
}

void
SceneManager::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    if (target == targets()[0])
        throw std::logic_error("SceneManager must be on the root node only.");
}

void
SceneManager::nextFrame()
{
    ++_frameId;

    //_enterFrame->execute(shared_from_this());
    //_exitFrame->execute(shared_from_this());
}
