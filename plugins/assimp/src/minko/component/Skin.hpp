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

#include "minko/file/ASSIMPParser.hpp"

namespace minko
{
	namespace geometry
	{
		class Bone;
		
		class Skin:
			public std::enable_shared_from_this<Skin>
		{
		public:
			typedef std::shared_ptr<Skin>				Ptr;

		private:
			typedef std::shared_ptr<Bone>				BonePtr;
			typedef std::shared_ptr<math::Matrix4x4>	Matrix4x4Ptr;
			typedef std::vector<Matrix4x4Ptr>			Matrices4x4Ptr;

		private:
			std::vector<BonePtr>						_bones;

			float										_duration;				// in seconds
			std::vector<Matrices4x4Ptr>					_boneMatricesPerFrame;

			std::vector<unsigned int>					_numVertexBones;		// size = #vertices
			std::vector<unsigned int>					_vertexBones;			// size = #vertices * #bones      
			std::vector<float>							_vertexBoneWeights;		// size = #vertices * #bones   
			
		public:
			inline
			static
			Ptr
			create()
			{
				return std::shared_ptr<Skin>(new Skin());
			}

			inline
			std::vector<BonePtr>&
			bones()
			{
				return _bones;
			}

			BonePtr
			bone(unsigned int i);

			inline
			float
			duration() const
			{
				return _duration;
			}

			inline
			void
			duration(float value)
			{
				_duration = value;
			}

			void
			reorganizeByVertices();

		private:
			Skin();

			void
			clear();
			
			unsigned short
			lastVertexId() const;
		};
	}
}
