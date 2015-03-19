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

#include "minko/component/AbstractAnimation.hpp"
#include "minko/component/Animation.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/POPGeometryLodScheduler.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/POPGeometryWriterPreprocessor.hpp"
#include "minko/geometry/Bone.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/Skin.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::scene;

POPGeometryWriterPreprocessor::POPGeometryWriterPreprocessor() :
    AbstractWriterPreprocessor<Node::Ptr>()
{
}

void
POPGeometryWriterPreprocessor::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    // TODO
    // introduce heuristics based on scene layout
    // * find object instances within scene hierarchy
    //   to dispatch lod scheduling components at correspondings object roots, used as
    //   scene object descriptors specifying type of technique to apply

    // by default whole scene is streamed as a progressive ordered mesh
    node->addComponent(POPGeometryLodScheduler::create());

    auto animatedNodes = collectAnimatedNodes(node);

    markPOPGeometries(node, animatedNodes);
}

void
POPGeometryWriterPreprocessor::markPOPGeometries(Node::Ptr root, std::unordered_set<Node::Ptr> ignoredNodes)
{
    auto surfaceNodes = NodeSet::create(root)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    for (auto surfaceNode : surfaceNodes->nodes())
    {
        if (ignoredNodes.find(surfaceNode) != ignoredNodes.end())
            continue;

        for (auto surface : surfaceNode->components<Surface>())
        {
            auto geometry = surface->geometry();

            markPOPGeometry(surfaceNode, surface, geometry);
        }
    }
}

void
POPGeometryWriterPreprocessor::markPOPGeometry(Node::Ptr node, Surface::Ptr surface, Geometry::Ptr geometry)
{
    geometry->data()->set(std::string("type"), std::string("pop"));
}

std::unordered_set<Node::Ptr>
POPGeometryWriterPreprocessor::collectAnimatedNodes(Node::Ptr root)
{
    auto animatedNodes = std::unordered_set<Node::Ptr>();

    auto abstractAnimationNodes = NodeSet::create(root)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<AbstractAnimation>(); });

    for (auto animatedNode : abstractAnimationNodes->nodes())
    {
        auto animatedNodeDescendants = NodeSet::create(animatedNode)
            ->descendants(true);

        animatedNodes.insert(animatedNodeDescendants->nodes().begin(), animatedNodeDescendants->nodes().end());
    }

    auto skinningNodes = NodeSet::create(root)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Skinning>(); });

    for (auto skinningNode : skinningNodes->nodes())
    {
        auto skin = skinningNode->component<Skinning>()->skin();

        for (auto i = 0u; i < skin->numBones(); ++i)
        {
            auto bone = skin->bone(i);

            auto boneDescendants = NodeSet::create(bone->node())
                ->descendants(true);

            animatedNodes.insert(boneDescendants->nodes().begin(), boneDescendants->nodes().end());
        }
    }

    return animatedNodes;
}
