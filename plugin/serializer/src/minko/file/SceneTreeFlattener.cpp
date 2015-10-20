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
#include "minko/component/Metadata.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/SceneTreeFlattener.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::scene;

SceneTreeFlattener::SceneTreeFlattener() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _protectedNodePredicateFunction(),
    _progressRate(0.f),
    _statusChanged(StatusChangedSignal::create())
{
}

void
SceneTreeFlattener::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "SceneTreeFlattener: start");

    auto retargetedSurfaces = std::list<RetargetedSurface>();

    collapseNode(node, nullptr, node, retargetedSurfaces);

    patchNode(node, retargetedSurfaces);

    _progressRate = 1.f;

    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "SceneTreeFlattener: stop");
}

bool
SceneTreeFlattener::collapseNode(Node::Ptr                      node,
                                 Node::Ptr                      parent,
                                 Node::Ptr                      root,
                                 std::list<RetargetedSurface>&  retargetedSurfaces)
{
    auto protectedDescendant = false;

    auto childrenToRemove = std::list<Node::Ptr>();

    for (auto child : node->children())
    {
        auto childRetargetedSurfaces = std::list<RetargetedSurface>();

        auto childProtectedDescendant = collapseNode(child, node, root, childRetargetedSurfaces);

        if (!childProtectedDescendant)
            childrenToRemove.push_back(child);

        protectedDescendant |= childProtectedDescendant;

        retargetedSurfaces.splice(retargetedSurfaces.end(), childRetargetedSurfaces);
    }

    for (auto child : childrenToRemove)
        node->removeChild(child);

    const auto surfaces = node->components<Surface>();
    const auto transforms = node->components<Transform>();

    const auto nodeIsProtected =
        node == root ||
        protectedDescendant ||
        (_protectedNodePredicateFunction ? _protectedNodePredicateFunction(node) : defaultProtectedNodePredicateFunction(node));

    if (!nodeIsProtected)
    {
        const auto localTransformMatrix = transforms.empty()
            ? math::mat4()
            : transforms.front()->matrix();

        auto nodeTransform = transforms.front();

        for (auto& retargetedSurface : retargetedSurfaces)
        {
            retargetedSurface.matrix = localTransformMatrix * retargetedSurface.matrix;
        }

        for (auto surface : surfaces)
        {
            retargetedSurfaces.emplace_back(surface, localTransformMatrix);
        }
    }
    else
    {
        patchNode(node, retargetedSurfaces);

        retargetedSurfaces.clear();
    }

    return nodeIsProtected;
}

void
SceneTreeFlattener::patchNode(Node::Ptr node, const std::list<RetargetedSurface>& retargetedSurfaces)
{
    for (const auto& retargetedSurface : retargetedSurfaces)
    {
        auto surfaceNode = Node::create(retargetedSurface.surface->target()->name())
            ->addComponent(Transform::create(retargetedSurface.matrix))
            ->addComponent(BoundingBox::create())
            ->addComponent(retargetedSurface.surface);

        node->addChild(surfaceNode);
    }
}

bool
SceneTreeFlattener::defaultProtectedNodePredicateFunction(Node::Ptr node)
{
    return node->components().size() >
        node->components<Transform>().size() +
        node->components<BoundingBox>().size() +
        node->components<Surface>().size() +
        node->components<Metadata>().size() ||
        (node->data().hasProperty("animated") && node->data().get<bool>("animated"));
}
