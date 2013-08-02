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

		enum MkTypes
		{
			BOOLEAN					= 0,
			NUMBER 					= 1,
			POINT					= 2,
			VECTOR4					= 3,
			VECTOR4B				= 9,
			MATRIX4X4 				= 4,
			VECTORN					= 5,
			VECTORV4				= 6,
			VECTORM4X4				= 7,
			TEXTURE_RESOURCE		= 8,

			GROUP					= 0,
			MESH					= 1,
			SYMBOL					= 2,
			LIGHT					= 3,
			CAMERA 					= 4,

			DIRECTIONAL_LIGHT		= 10,
			POINT_LIGHT				= 11,
			SPOT_LIGHT				= 12,
			AMBIENT_LIGHT			= 13,

			ANIMATION_CTRL			= 20,
			SKELETON_CTRL			= 21,
			SKINNING_CTRL			= 22,
			MASTER_ANIMATION_CTRL	= 23,

			PROJECTION_CAMERA 		= 30,
			ORTHOGRAPHIC_CAMERA		= 31,

			TEXTURE_ASSET			= 40,
			SYMBOL_ASSET			= 41,
			EFFECT_ASSET			= 42,
			MATERIAL_ASSET			= 43,
			TAGS_ASSET				= 44,
			SOUND_ASSET				= 45,

			MATRIX_SEGMENT 			= 80,
			MATRIX_LINEAR			= 81,
			SCALAR_LINEAR			= 82,
			SCALAR_REGULAR			= 83,
			COLOR_LINEAR			= 84,
			MATRIX_REGULAR			= 85,

		};
	}
}
