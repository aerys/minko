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
