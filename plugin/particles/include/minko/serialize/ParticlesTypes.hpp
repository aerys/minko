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
	namespace serialize
	{
		enum class SamplerId
		{
            UNKNOWN         = 0,
			CONSTANT_COLOR  = 2,
			CONSTANT_NUMBER = 1,
			LINEAR_COLOR	= 3,
			LINEAR_NUMBER   = 4,
			RANDOM_COLOR	= 6,
			RANDOM_NUMBER	= 7
		};

		enum class EmitterShapeId
		{
            UNKNOWN     = 0,
            CYLINDER    = 1,
            CONE        = 2,
            SPHERE      = 3,
            POINT       = 4,
            BOX         = 5
		};

		enum class ModifierId
		{
            START_COLOR	            = 0,
			START_FORCE			    = 1,
			START_ROTATION	        = 2,
			START_SIZE			    = 3,
			START_SPRITE		    = 4,
			START_VELOCITY			= 5,
			START_ANGULAR_VELOCITY	= 12,
			COLOR_BY_SPEED		    = 6,
            COLOR_OVER_TIME         = 7,
            FORCE_OVER_TIME         = 8,    
            SIZE_BY_SPEED           = 9,
            SIZE_OVER_TIME          = 10,
            VELOCITY_OVER_TIME      = 11
		};
	}
}
