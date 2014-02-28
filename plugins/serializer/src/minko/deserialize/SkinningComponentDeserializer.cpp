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

#include "SkinningComponentDeserializer.hpp"

#include "minko/geometry/Bone.hpp"
#include "minko/geometry/Skin.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Animation.hpp"
#include "minko/animation/Matrix4x4Timeline.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Quaternion.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/file/Options.hpp"
#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::animation;
using namespace minko::deserialize;

/*static*/
Skinning::Ptr
SkinningComponentDeserializer::computeSkinning(file::Options::Ptr						options,
                                               render::AbstractContext::Ptr             context,
											   const std::vector<geometry::Bone::Ptr>&	bones, 
											   Node::Ptr								skeletonRoot)
{
	unsigned int	framerate	= options->skinningFramerate();
	SkinningMethod	method		= options->skinningMethod();

	if (!haveBonesCommonRoot(bones, skeletonRoot))
		throw new std::logic_error("Sone bones are not connected to the specified skeleton root.");

    std::cout 
        << "# abstract animations under skeleton root = " 
        << NodeSet::create(skeletonRoot)->descendants(true)->where([](Node::Ptr n){ return n->hasComponent<AbstractAnimation>(); })->nodes().size() 
        << std::endl;
    std::cout 
        << "# animations under skeleton root = " 
        << NodeSet::create(skeletonRoot)->descendants(true)->where([](Node::Ptr n){ return n->hasComponent<Animation>(); })->nodes().size() 
        << std::endl;

    NodeTransformTimeline   nodeToTimelines;
    NodeMatrices            nodeToFrameMatrices;

    const unsigned int duration = collectAnimations(
        bones, 
        skeletonRoot, 
        nodeToTimelines
    );

    const unsigned int numFrames = sampleAnimations(
        options, 
        bones, 
        skeletonRoot, 
        duration, 
        nodeToTimelines,
        nodeToFrameMatrices
    );

    auto skin = geometry::Skin::create(bones.size(), duration, numFrames);

    std::vector<Matrix4x4::Ptr> matrices(numFrames, nullptr);
    for (auto& m : matrices)
        m = Matrix4x4::create();

    for (unsigned int boneId = 0; boneId < bones.size(); ++boneId)
    {
        skin->bone(boneId, bones[boneId]);

        precomputeModelToRootMatrices(
            bones[boneId],
            skeletonRoot,
            nodeToFrameMatrices,
            matrices
        );

        for (unsigned int frameId = 0; frameId < numFrames; ++frameId)
            skin->matrix(frameId, boneId, matrices[frameId]);
    }

    // strip the scene nodes below the skeleton off their obsolete
    // Transform and Animation components.
    cleanNode(skeletonRoot, false);

	return Skinning::create(
        skin->reorganizeByVertices()->transposeMatrices()->disposeBones(),
        options->skinningMethod(),
        context,
        std::vector<Animation::Ptr>()
    );
}

/*static*/
void
SkinningComponentDeserializer::cleanNode(Node::Ptr node, bool self)
{
    if (self)
    {
        if (node->hasComponent<Transform>())
            node->removeComponent(node->component<Transform>());
        if (node->hasComponent<Animation>())
            node->removeComponent(node->component<Animation>());
    }
    for (auto& n : node->children())
        cleanNode(n, true);
}

/*static*/
void
SkinningComponentDeserializer::precomputeModelToRootMatrices(geometry::Bone::Ptr            bone, 
                                                             Node::Ptr                      skeletonRoot, 
                                                             const NodeMatrices&            nodeToFrameMatrices,
                                                             std::vector<Matrix4x4::Ptr>&   matrices)
{
    const unsigned int numFrames = matrices.size();

    for (auto& m : matrices)
        m->copyFrom(bone->offsetMatrix());

    auto currentNode = bone->node();
    do
    {
        if (currentNode == nullptr)
            break;

        const auto foundMatricesIt = nodeToFrameMatrices.find(currentNode);
        if (foundMatricesIt != nodeToFrameMatrices.end())
        {
            assert(foundMatricesIt->second.size() == numFrames);

            for (unsigned int frameId = 0; frameId < numFrames; ++frameId)
                matrices[frameId]->append(foundMatricesIt->second[frameId]);
        }
        else if (currentNode->hasComponent<Transform>())
            for (auto& m : matrices)
                m->append(currentNode->component<Transform>()->matrix());

        currentNode = currentNode->parent();
    }
    while(currentNode != skeletonRoot);
}

/*static*/
unsigned int
SkinningComponentDeserializer::sampleAnimations(file::Options::Ptr						options,
                                                const std::vector<geometry::Bone::Ptr>& bones, 
                                                Node::Ptr                               skeletonRoot,
                                                unsigned int                            duration, // expected in milliseconds
                                                const NodeTransformTimeline&            nodeToTimelines,
                                                NodeMatrices&                           nodeToFrameMatrices)
{
    nodeToFrameMatrices.clear();

    const unsigned int  numFrames   = (unsigned int)std::max(2, int(floorf(options->skinningFramerate() * duration * 1e-3f)));
    const float         timeStep    = duration / float(numFrames - 1);

    std::cout << "Skinning deserializetion\nduration = " << duration << " (framerate = " << options->skinningFramerate() << ") -> " << numFrames << " frames" << std::endl;

    for (auto& nodeAndTimeline : nodeToTimelines)
    {
        auto node       = nodeAndTimeline.first;
        auto timeline   = nodeAndTimeline.second;

        // predecompose matrices for better interpolation
        nodeToFrameMatrices[node]   = std::vector<Matrix4x4::Ptr>(numFrames, nullptr);

        auto&   matrices    = nodeToFrameMatrices[node];
        float   time        = 0.0f;
        for (unsigned int frameId = 0; frameId < numFrames; ++frameId)
        {
            matrices[frameId]   = timeline->interpolate((unsigned int)floorf(time));
            time                += timeStep;
        }
    }

    return numFrames;
}

/*static*/
unsigned int
SkinningComponentDeserializer::collectAnimations(const std::vector<geometry::Bone::Ptr>&	bones, 
												 Node::Ptr									skeletonRoot,
												 NodeTransformTimeline&						nodeToTimelines)
{
	unsigned int duration = 0;
	nodeToTimelines.clear();

	for (auto& bone : bones)
	{
		auto currentNode = bone->node();
		do
		{
			if (!currentNode)
				break;

			if (currentNode->hasComponent<Animation>())
			{
				auto animation = currentNode->component<Animation>();

				for (auto& timeline : animation->timelines())
					if (timeline->propertyName() == "transform.matrix")
					{
						duration                        = std::max(duration, timeline->duration());
                        auto matrixTimeline             = std::dynamic_pointer_cast<Matrix4x4Timeline>(timeline);
                        assert(matrixTimeline);

				        nodeToTimelines[currentNode]    = matrixTimeline;

                        break;
					}
			}
			currentNode = currentNode->parent();
		}
		while (currentNode != skeletonRoot);
	}

	return duration;
}

/*static*/
bool
SkinningComponentDeserializer::haveBonesCommonRoot(const std::vector<geometry::Bone::Ptr>&	bones, 
												   Node::Ptr								skeletonRoot)
{
	if (bones.empty())
		return false;

	bool hasCommonRoot = true;
	for (auto& bone : bones)
		hasCommonRoot = 
			hasCommonRoot 
			&&
			!NodeSet::create(bone->node())->ancestors(true)->where([=](Node::Ptr n){ return n == skeletonRoot; })->nodes().empty();

	return hasCommonRoot;
}