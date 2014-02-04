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

#include "minko/Color.hpp"

#include "minko/math/Vector4.hpp"

using namespace minko;
using namespace minko::math;

Vector4::Ptr
Color::hslaToRgba(float h, float s, float l, float a, Vector4::Ptr out)
{
	float r, g, b;

	if (s == 0)
		r = g = b = l; // achromatic
	else
	{
		float q = l < 0.5f ? l * (1.f + s) : l + s - l * s;
		float p = 2.f * l - q;

		r = hueToRgb(p, q, h + 1.f / 3.f);
		g = hueToRgb(p, q, h);
		b = hueToRgb(p, q, h - 1.f / 3.f);
	}

	return Vector4::create(r, g, b, a);
}

float
Color::hueToRgb(float p, float q, float t)
{
	if (t < 0.f)
		t += 1.f;
	if (t > 1.f)
		t -= 1.f;
	if (t < 1.f / 6.f)
		return p + (q - p) * 6.f * t;
	if (t < 1.f / 2.f)
		return q;
	if (t < 2.f / 3.f)
		return p + (q - p) * (2.f / 3.f - t) * 6.f;

	return p;
}

Vector4::Ptr
Color::uintToVec4(uint rgba, Vector4::Ptr output)
{
	static const float div255 = 1.0f / 255.0f;

	const float r = ((rgba >> 24) & 0xff) * div255;
	const float g = ((rgba >> 16) & 0xff) * div255;
	const float b = ((rgba >> 8)  & 0xff) * div255;
	const float a = ( rgba        & 0xff) * div255;

	if (output == nullptr)
		output = Vector4::create();
	
	return output->setTo(r, g, b, a);
}