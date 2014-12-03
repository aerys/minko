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
			typedef std::shared_ptr<component::Animation>			AnimationPtr;
            typedef std::vector<math::mat4>                         MatrixVector;
            typedef std::unordered_map<NodePtr, MatrixVector>       NodeMatrices;
            typedef std::unordered_map<NodePtr, MatrixTimelinePtr>  NodeTransformTimeline;

        public:
			static const std::string PNAME_TRANSFORM;

            static 
            SkinningPtr
            computeSkinning(OptionsPtr, 
							AbsContextPtr, 
							const std::vector<BonePtr>&, 
							const std::vector<NodePtr>&,
							NodePtr);

        private:
            static
            unsigned int
            collectAnimations(const std::vector<BonePtr>&, 
							  const std::vector<NodePtr>&,
							  NodePtr, 
							  NodeTransformTimeline&);

            static
            unsigned int
            sampleAnimations(OptionsPtr, 
							 const std::vector<BonePtr>&, 
							 NodePtr, 
							 unsigned int duration, 
							 const NodeTransformTimeline&, 
							 NodeMatrices&);

            static
            void
            precomputeModelToRootMatrices(NodePtr, 
										  NodePtr, 
										  const NodeMatrices&, 
										  std::vector<math::mat4>&);

			static 
			void
			computeSurfaceAnimations(unsigned int duration, 
									 unsigned int numFrames, 
									 NodePtr, 
									 const std::vector<BonePtr>&, 
									 const std::vector<NodePtr>&,
									 const NodeMatrices&,
									 std::vector<AnimationPtr>&);

			static
			void
			clean(NodePtr);

			static
			void
			removeAnimations(NodePtr);

            static
            bool
            haveBonesCommonRoot(const std::vector<BonePtr>&, 
								const std::vector<NodePtr>&,
								NodePtr);
        };
    }
}