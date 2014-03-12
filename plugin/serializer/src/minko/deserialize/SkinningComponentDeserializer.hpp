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

#pragma once

#include "minko/Common.hpp"

namespace minko
{
    namespace geometry
    {
        class Bone;
    }

    namespace deserialize
    {
        class SkinningComponentDeserializer
        {
        public:
            typedef std::shared_ptr<SkinningComponentDeserializer>  Ptr;

        private:
            typedef std::shared_ptr<component::Skinning>            SkinningPtr;
            typedef std::shared_ptr<geometry::Bone>                 BonePtr;
            typedef std::shared_ptr<file::Options>                  OptionsPtr;
            typedef std::shared_ptr<scene::Node>                    NodePtr;
            typedef std::shared_ptr<animation::Matrix4x4Timeline>   MatrixTimelinePtr;
            typedef std::shared_ptr<render::AbstractContext>        AbsContextPtr;

            typedef std::vector<std::shared_ptr<math::Matrix4x4>>   Matrices4x4Ptr;
            typedef std::unordered_map<NodePtr, Matrices4x4Ptr>     NodeMatrices;
            typedef std::unordered_map<NodePtr, MatrixTimelinePtr>  NodeTransformTimeline;

        public:
            static 
            SkinningPtr
            computeSkinning(OptionsPtr, AbsContextPtr, const std::vector<BonePtr>&, NodePtr);

        private:
            static
            unsigned int
            collectAnimations(const std::vector<BonePtr>&, NodePtr, NodeTransformTimeline&);

            static
            unsigned int
            sampleAnimations(OptionsPtr, const std::vector<BonePtr>&, NodePtr, unsigned int duration, const NodeTransformTimeline&, NodeMatrices&);

            static
            void
            precomputeModelToRootMatrices(BonePtr, NodePtr, const NodeMatrices&, Matrices4x4Ptr&);

            static
            void
            cleanNode(NodePtr, bool);

            static
            bool
            haveBonesCommonRoot(const std::vector<BonePtr>&, NodePtr);
        };
    }
}