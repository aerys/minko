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
	namespace component
	{
		class ParticleSystem;
	}

    namespace data
    {
        class ParticlesProvider;
    }

	namespace geometry
	{
		class ParticlesGeometry;
	}
	namespace render
	{
		class ParticleVertexBuffer;
		class ParticleIndexBuffer;
	}

	namespace particle
	{
		struct ParticleData;
		enum class StartDirection;

		namespace modifier
		{
			class IParticleModifier;

			class IParticleInitializer;
			class StartVelocity;
			class StartForce;
			class StartColor;
			class StartSize;
			class StartRotation;
			class StartAngularVelocity;
			class StartSprite;

			class IParticleUpdater;
			class VelocityOverTime;
			class ForceOverTime;
			class ColorOverTime;
			class SizeOverTime;
			class ColorBySpeed;
			class SizeBySpeed;
		}

		namespace sampler
		{
			template<class T>
			class Sampler;

			template<class T>
			class Constant;
			template<class T>
			class RandomValue;
			template<class T>
			class LinearlyInterpolatedValue;
		}

		namespace shape
		{
			class EmitterShape;
			class Box;
			class Cone;
			class Cylinder;
			class Sphere;
			class Point;
		}

		namespace tools
		{
			enum class VertexComponentFlags;

			template<class T>
			class Vector3;

			template <class T>
			class Modifier1;
			template <class T>
			class Modifier3;
		}
	}
}