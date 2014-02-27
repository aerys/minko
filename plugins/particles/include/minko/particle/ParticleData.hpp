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

#include "minko/ParticlesCommon.hpp"

namespace minko
{
	namespace particle
	{
		struct ParticleData
		{
			bool	alive;

			float	x;
			float	y;
			float	z;

			float	oldx;
			float	oldy;
			float	oldz;

			float	startvx;
			float	startvy;
			float	startvz;

			float	startfx;
			float	startfy;
			float	startfz;

			float	r;
			float	g;
			float	b;

			float	size;

			float	rotation;
			float	startAngularVelocity;

			float 	lifetime;
			float	timeLived;

			float	spriteIndex;

			ParticleData() : alive (false),
				x (0),
				y (0),
				z (0),
				startvx (0),
				startvy (0),
				startvz (0),
				startfx (0),
				startfy (0),
				startfz (0),
				r (1),
				g (1),
				b (1),
				size (1),
				rotation (0),
				lifetime (0),
				timeLived (0),
				spriteIndex (0) {};
		};
	}
}
