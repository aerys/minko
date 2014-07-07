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

#include "minko/render/DrawCall.hpp"

using namespace minko;
using namespace minko::render;

const unsigned int	DrawCall::MAX_NUM_TEXTURES		= 8;
const unsigned int	DrawCall::MAX_NUM_VERTEXBUFFERS	= 8;

DrawCall::DrawCall(Pass::Ptr pass) :
	_pass(pass)
{
	if (_pass == nullptr)
		throw std::invalid_argument("pass");
}

void
DrawCall::initialize()
{
}

void
DrawCall::bind(ContainerPtr rootData, ContainerPtr rendererData, ContainerPtr targetData)
{
    // FIXME
}

void
DrawCall::bindIndexBuffer()
{
    // FIXME
}

void
DrawCall::bindVertexAttribute(const std::string&	inputName,
							  int					location,
							  uint					vertexBufferIndex)
{
    // FIXME
}

void
DrawCall::bindTextureSampler(const std::string&		inputName,
							 int					location,
							 uint					textureIndex,
   							 const SamplerState&	samplerState)
{
    // FIXME
}

void
DrawCall::bindUniform(const std::string&	inputName,
					  ProgramInputs::Type	type,
					  int					location)
{
    // FIXME
}

void
DrawCall::bindStates()
{
    // FIXME
}

void
DrawCall::render(const AbstractContext::Ptr& context, AbstractTexture::Ptr renderTarget)
{
	// FIXME
}
