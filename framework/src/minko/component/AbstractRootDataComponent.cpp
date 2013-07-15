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

#include "AbstractRootDataComponent.hpp"

#include "minko/scene/Node.hpp"
#include "minko/data/Container.hpp"

using namespace minko::component;

AbstractRootDataComponent::AbstractRootDataComponent() :
    _data(data::Provider::create())
{
}

void
AbstractRootDataComponent::initialize()
{
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &AbstractRootDataComponent::targetAddedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _targetRemovedSlot = targetRemoved()->connect(std::bind(
        &AbstractRootDataComponent::targetRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2
    ));
}

void
AbstractRootDataComponent::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
    auto cb = std::bind(
        &AbstractRootDataComponent::addedOrRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );

    _addedSlot = target->added()->connect(cb);
    _removedSlot = target->removed()->connect(cb);

    _root = target->root();
    _root->data()->addProvider(_data);
}

void
AbstractRootDataComponent::targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
    _root = nullptr;
}

void
AbstractRootDataComponent::addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    _root->data()->removeProvider(_data);
    _root = target->root();
    _root->data()->addProvider(_data);
}
