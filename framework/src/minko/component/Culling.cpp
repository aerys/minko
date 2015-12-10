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

#include "minko/component/BoundingBox.hpp"
#include "minko/component/Culling.hpp"
#include "minko/scene/Node.hpp"
#include "minko/data/Store.hpp"
#include "minko/math/Frustum.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/math/OctTree.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::component;

Culling::Culling(ShapePtr shape, const std::string& bindProperty, scene::Layout layout) :
    AbstractComponent(),
    _frustum(shape),
    _bindProperty(bindProperty),
    _worldSize(50.f),
    _maxDepth(7u),
    _layout(layout)
{
}

void
Culling::targetAdded(NodePtr target)
{
    if (target->components<Culling>().size() > 1)
        throw std::logic_error("The same camera node cannot have more than one Culling.");

    if (_octTree == nullptr)
        _octTree = math::OctTree::create(worldSize(), maxDepth(), math::vec3(0.f));

    if (target->root()->hasComponent<SceneManager>())
        targetAddedToSceneHandler(nullptr, target, nullptr);
    else
        _addedToSceneSlot = target->added().connect(
            [this](NodePtr node, NodePtr target, NodePtr ancestor)
            {
                targetAddedToSceneHandler(node, target, ancestor);
            }
        );

    _viewMatrixChangedSlot = target->data().propertyChanged(_bindProperty).connect(
        [this](data::Store& d, data::Provider::Ptr p, const String& n)
        {
            _updateNextFrame = true;
        }
    );
}

void
Culling::targetRemoved(NodePtr target)
{
    _addedSlot          = nullptr;
    _layoutChangedSlot  = nullptr;
    _renderingBeginSlot = nullptr;
}

void
Culling::targetAddedToSceneHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    auto sceneManager = target->root()->component<SceneManager>();

    if (sceneManager)
    {
        _addedToSceneSlot = nullptr;

        _layoutChangedSlot = target->root()->layoutChanged().connect(
            [this](NodePtr node, NodePtr target)
            {
                layoutChangedHandler(node, target);
            }
        );

        _addedSlot = target->root()->added().connect(
            [this](NodePtr node, NodePtr target, NodePtr ancestor)
            {
                addedHandler(node, target, ancestor);
            },
            -1.f
        );

        _removedSlot = target->root()->removed().connect(
            [this](NodePtr node, NodePtr target, NodePtr ancestor)
            {
                removedHandler(node, target, ancestor);
            }
        );

        _renderingBeginSlot = sceneManager->renderingBegin()->connect(
            [this](SceneManager::Ptr sm, uint fid, render::AbstractTexture::Ptr rt)
            {
                if (_updateNextFrame)
                {
                    _frustum->updateFromMatrix(this->target()->data().get<math::mat4>(_bindProperty));

                    _octTree->testFrustum(
                        _frustum,
                        [&](NodePtr node)
                        {
                            auto layout = node->layout();

                            if ((layout & scene::BuiltinLayout::HIDDEN) == 0u)
                                layout = layout | scene::BuiltinLayout::DEFAULT;
                            layout = layout | scene::BuiltinLayout::INSIDE_FRUSTUM;

                            node->layout(layout);
                        },
                        [&](NodePtr node)
                        {
                            auto layout = node->layout();

                            layout = layout & ~scene::BuiltinLayout::DEFAULT;
                            layout = layout & ~scene::BuiltinLayout::INSIDE_FRUSTUM;

                            node->layout(layout);
                        }
                    );

                    _updateNextFrame = false;
                }
            },
            -1.f
        );

        addedHandler(target->root(), target->root(), target->root());
    }
}

void
Culling::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    scene::NodeSet::Ptr nodeSet = scene::NodeSet::create(target)
        ->descendants(true)
        ->where([&](NodePtr descendant)
        {
            return (descendant->layout() & scene::BuiltinLayout::IGNORE_CULLING) == 0
                && descendant->hasComponent<Surface>();
        });

    for (auto n : nodeSet->nodes())
        _octTree->insert(n);
}

void
Culling::removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    auto nodeSet = scene::NodeSet::create(target)
        ->descendants(true)
        ->where([](NodePtr descendant)
    {
        return (descendant->layout() & scene::BuiltinLayout::IGNORE_CULLING) == 0
            && descendant->hasComponent<Surface>();
    });

    for (auto nodeToRemove : nodeSet->nodes())
        _octTree->remove(nodeToRemove);
}

void
Culling::layoutChangedHandler(NodePtr node, NodePtr target)
{
    if ((target->layout() & scene::BuiltinLayout::IGNORE_CULLING) == 0)
        _octTree->insert(target);
    else
        _octTree->remove(target);
}
