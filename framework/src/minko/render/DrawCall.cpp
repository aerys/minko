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

#include "minko/data/Container.hpp"

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

DrawCall::ContainerPtr
DrawCall::getContainer(ContainerPtr         rootData,
                       ContainerPtr         rendererData,
                       ContainerPtr         targetData,
                       data::BindingSource  source,
                       const std::string&   name)
{
    switch (source)
    {
        case data::BindingSource::ROOT:
            return rootData;
        case data::BindingSource::RENDERER:
            return rendererData;
        case data::BindingSource::TARGET:
            return targetData;
    }

    return nullptr;
}

void
DrawCall::bind(ContainerPtr         rootData,
               ContainerPtr         rendererData,
               ContainerPtr         targetData,
               ProgramInputs::Ptr   inputs)
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
DrawCall::bindUniform(ContainerPtr          container,
                      const std::string&	inputName,
					  ProgramInputs::Type	type,
					  int					location)
{
    switch (type)
    {
        case ProgramInputs::Type::int1:
            _uniformInt.push_back({ location, 1, container->getPointer<int>(inputName) });
            break;
        case ProgramInputs::Type::int2:
            _uniformInt.push_back({ location, 2, math::value_ptr(container->get<math::ivec2>(inputName)) });
            break;
        case ProgramInputs::Type::int3:
            _uniformInt.push_back({ location, 3, math::value_ptr(container->get<math::ivec3>(inputName)) });
            break;
        case ProgramInputs::Type::int4:
            _uniformInt.push_back({ location, 4, math::value_ptr(container->get<math::ivec4>(inputName)) });
            break;
        case ProgramInputs::Type::float1:
            _uniformFloat.push_back({ location, 1, container->getPointer<float>(inputName) });
            break;
        case ProgramInputs::Type::float2:
            _uniformFloat.push_back({ location, 2, math::value_ptr(container->get<math::vec4>(inputName)) });
            break;
        case ProgramInputs::Type::float3:
            _uniformFloat.push_back({ location, 3, math::value_ptr(container->get<math::vec4>(inputName)) });
            break;
        case ProgramInputs::Type::float4:
            _uniformFloat.push_back({ location, 4, math::value_ptr(container->get<math::vec4>(inputName)) });
            break;
        case ProgramInputs::Type::float16:
            _uniformFloat.push_back({ location, 16, math::value_ptr(container->get<math::mat4>(inputName)) });
            break;
    }
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
