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

#include "minko/render/Program.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/render/Shader.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/IndexBuffer.hpp"


using namespace minko;
using namespace minko::render;

Program::Program(Program::AbstractContextPtr context) :
	AbstractResource(context),
	_indexBuffer(nullptr)
{
}

void
Program::upload()
{
	_id = context()->createProgram();
	_context->attachShader(_id, _vertexShader->id());
	_context->attachShader(_id, _fragmentShader->id());
	_context->linkProgram(_id);

	_inputs = _context->getProgramInputs(_id);
}

void
Program::dispose()
{
	_context->deleteProgram(_id);
	_id = -1;

	_vertexShader = nullptr;
	_fragmentShader = nullptr;
}

void
Program::setUniform(const std::string& name, std::shared_ptr<render::AbstractTexture> texture)
{
	if (!_inputs->hasName(name))
		return;

	_textures[_inputs->location(name)] = texture;
}

void
Program::setVertexAttribute(const std::string& name, unsigned int attributeSize, const std::vector<float>& data)
{
	if (!_inputs->hasName(name))
		return;
	
	auto vertexBuffer = VertexBuffer::create(_context, data);
	vertexBuffer->addAttribute(name, attributeSize, 0);

	_vertexBuffers[_inputs->location(name)] = vertexBuffer;
}

void
Program::setIndexBuffer(const std::vector<unsigned short>& indices)
{
	if (indices.empty())
	{
		_indexBuffer = nullptr;

		return;
	}

	_indexBuffer = IndexBuffer::create(_context, indices);
}