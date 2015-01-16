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

#include "minko/geometry/QuadGeometry.hpp"

#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;

QuadGeometry::QuadGeometry(uint		numColumns,
						   uint		numRows,
						   float	width,
						   float	height):
	Geometry("quad_" + std::to_string(numColumns) + "x" + std::to_string(numRows)),
	_numColumns(numColumns),
	_numRows(numRows),
	_width(width),
	_height(height)
{
}

void
QuadGeometry::initialize(std::shared_ptr<render::AbstractContext> context)
{
	std::vector<float> vertexData;
	std::vector<unsigned short> indicesData;

	for (uint y = 0; y <= _numRows; ++y)
	{
		for (uint x = 0; x <= _numColumns; ++x)
		{
			vertexData.push_back((float(x) / float(_numColumns) - 0.5f) * _width);
			vertexData.push_back((float(y) / float(_numRows) - 0.5f) * _height);
			vertexData.push_back(0.f);
			vertexData.push_back(0.f);
			vertexData.push_back(0.f);
			vertexData.push_back(1.f);
			vertexData.push_back(float(x) / float(_numColumns));
			vertexData.push_back(1.f - float(y) / float(_numRows));
		}
	}

	for (uint y = 0; y < _numRows; y++)
	{
		for (uint x = 0; x < _numColumns;  x++)
		{
				indicesData.push_back(x + (_numColumns + 1) * y);
				indicesData.push_back(x + 1 + y * (_numColumns + 1));
				indicesData.push_back((y + 1) * (_numColumns + 1) + x);
				indicesData.push_back(x + 1 + y * (_numColumns + 1));
				indicesData.push_back((y + 1) * (_numColumns + 1) + x + 1);
				indicesData.push_back((y + 1) * (_numColumns + 1) + x);
		}
	}

	auto vertexBuffer	= render::VertexBuffer::create(context, vertexData);
	auto indexBuffer	= render::IndexBuffer::create(context, indicesData);

	vertexBuffer->addAttribute("position", 3, 0);
	vertexBuffer->addAttribute("normal", 3, 3);
	vertexBuffer->addAttribute("uv", 2, 6);
    addVertexBuffer(vertexBuffer);

    indices(indexBuffer);

    computeCenterPosition();
}
