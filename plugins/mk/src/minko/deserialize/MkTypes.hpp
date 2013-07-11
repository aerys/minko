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
	namespace deserialize
	{

		class MkTypes
		{
		public:

			static const unsigned BOOLEAN					= 0;
			static const unsigned NUMBER 					= 1;
			static const unsigned POINT						= 2;
			static const unsigned VECTOR4					= 3;
			static const unsigned MATRIX4X4 				= 4;
			static const unsigned VECTORN					= 5;
			static const unsigned VECTORV4					= 6;
			static const unsigned VECTORM4X4				= 7;
			static const unsigned TEXTURE_RESOURCE			= 8;

			static const unsigned int GROUP					= 0;
			static const unsigned int MESH					= 1;
			static const unsigned int SYMBOL				= 2;
			static const unsigned int LIGHT					= 3;
			static const unsigned int CAMERA 				= 4;

			static const unsigned int DIRECTIONAL_LIGHT		= 10;
			static const unsigned int POINT_LIGHT			= 11;
			static const unsigned int SPOT_LIGHT			= 12;
			static const unsigned int AMBIENT_LIGHT			= 13;

			static const unsigned int ANIMATION_CTRL		= 20;
			static const unsigned int SKELETON_CTRL			= 21;
			static const unsigned int SKINNING_CTRL			= 22;
			static const unsigned int MASTER_ANIMATION_CTRL	= 23;

			static const unsigned int PROJECTION_CAMERA 	= 30;
			static const unsigned int ORTHOGRAPHIC_CAMERA	= 31;

			static const unsigned int TEXTURE_ASSET			= 40;
			static const unsigned int SYMBOL_ASSET			= 41;
			static const unsigned int EFFECT_ASSET			= 42;
			static const unsigned int MATERIAL_ASSET		= 43;
			static const unsigned int TAGS_ASSET			= 44;
			static const unsigned int SOUND_ASSET			= 45;

			static const unsigned int MATRIX_SEGMENT 		= 80;
			static const unsigned int MATRIX_LINEAR			= 81;
			static const unsigned int SCALAR_LINEAR			= 82;
			static const unsigned int SCALAR_REGULAR		= 83;
			static const unsigned int COLOR_LINEAR			= 84;
			static const unsigned int MATRIX_REGULAR		= 85;
		};
	}
}
