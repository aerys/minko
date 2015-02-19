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
		
		class Skin:
			public std::enable_shared_from_this<Skin>
		{
		public:
			typedef std::shared_ptr<Skin>	Ptr;

		private:
			typedef std::shared_ptr<Bone>	BonePtr;

		private:
			const unsigned int				        _numBones;
			std::vector<BonePtr>			        _bones;

			const uint						        _duration;				// in milliseconds
			const float						        _timeFactor;
			std::vector<std::vector<math::mat4>>	_boneMatricesPerFrame;

			unsigned int					        _maxNumVertexBones;
			std::vector<unsigned int>		        _numVertexBones;		// size = #vertices
			std::vector<unsigned int>		        _vertexBones;			// size = #vertices * #bones      
			std::vector<float>				        _vertexBoneWeights;		// size = #vertices * #bones   
			
		public:
			inline
			static
			Ptr
			create(unsigned int numBones, unsigned int duration, unsigned int numFrames)
			{
				return std::shared_ptr<Skin>(new Skin(numBones, duration, numFrames));
			}

			Ptr
			clone();

			inline
			unsigned int
			numBones() const
			{
				return _numBones;
			}

			inline
			unsigned int
			maxNumVertexBones() const
			{
				return _maxNumVertexBones;
			}

			inline
			std::vector<BonePtr>&
			bones()
			{
				return _bones;
			}

			inline
			void
			bones(std::vector<BonePtr> bones)
			{
				_bones = bones;
			}

			inline
			BonePtr
			bone(unsigned int boneId) const
			{
				return _bones[boneId];
			}

			inline
			void
			bone(unsigned int boneId, BonePtr value)
			{
				_bones[boneId] = value;
			}

			inline
			uint
			duration() const
			{
				return _duration;
			}

			uint
			getFrameId(uint) const;

			inline
			unsigned int
			numFrames() const
			{
				return _boneMatricesPerFrame.size();
			}

			inline
			void
			setBoneMatricesPerFrame(std::vector<std::vector<math::mat4>> boneMatricesPerFrame)
			{
				_boneMatricesPerFrame = boneMatricesPerFrame;
			}

			inline
            std::vector<std::vector<math::mat4>>
			getBoneMatricesPerFrame()
			{
				return _boneMatricesPerFrame;
			}

			inline
			const std::vector<math::mat4>&
			matrices(unsigned int frameId) const
			{
				return _boneMatricesPerFrame[frameId];
			}

			void
			matrix(unsigned int frameId, unsigned int boneId, const math::mat4&);

			inline
			unsigned int
			numVertices() const
			{
				return _numVertexBones.size();
			}

			inline
			unsigned int
			numVertexBones(unsigned int vertexId) const
			{
#ifdef DEBUG_SKINNING
				assert(vertexId < numVertices());
#endif // DEBUG_SKINNING

				return _numVertexBones[vertexId];
			}

			void
			vertexBoneData(unsigned int vertexId, unsigned int j, unsigned int& boneId, float& boneWeight) const;

			unsigned int
			vertexBoneId(unsigned int vertexId, unsigned int j) const;

			float 
			vertexBoneWeight(unsigned int vertexId, unsigned int j) const;

			Ptr
			reorganizeByVertices();

			Ptr
			disposeBones();

		private:
			Skin(unsigned int numBones, unsigned int duration, unsigned int numFrames);

			Skin(const Skin& skin);

			unsigned short
			lastVertexId() const;

			inline
			unsigned int
			vertexArraysIndex(unsigned int vertexId, unsigned int j) const
			{
#ifdef DEBUG_SKINNING
				assert(vertexId < numVertices() && j < numVertexBones(vertexId));
#endif // DEBUG_SKINNING

				return j + _numBones * vertexId;
			}

		};
	}
}
