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

#include "DrawCall.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/resource/VertexStream.hpp"
#include "minko/resource/IndexStream.hpp"
#include "minko/resource/Texture.hpp"
#include "minko/resource/Program.hpp"
#include "minko/data/Container.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/resource/VertexAttribute.hpp"

using namespace minko::math;
using namespace minko::render;
using namespace minko::resource;

DrawCall::DrawCall(std::shared_ptr<data::Container>						bindings,
				   const std::unordered_map<std::string, std::string>&	inputNameToBindingName) :
	_data(bindings),
	_inputNameToBindingName(inputNameToBindingName)
{
	bind(bindings);
}

void
DrawCall::bind(std::shared_ptr<data::Container> bindings)
{
	auto vertexSize		= _data->get<unsigned int>("geometry/vertex/size");
	auto indexStream	= bindings->get<IndexStream::Ptr>("geometry/indices");
	auto indexBuffer	= indexStream->id();
	auto numIndices		= indexStream->data().size();
	auto drawTriangles	= [=](AbstractContext::Ptr context)
	{
		context->drawTriangles(indexBuffer, numIndices);
	};
	
	for (auto passId = 0; bindings->hasProperty("effect/pass" + std::to_string(passId)); ++passId)
	{
		auto program		= bindings->get<Program::Ptr>("effect/pass" + std::to_string(passId));
		auto numTextures	= 0;
		auto programId		= program->id();

		_func.push_back([=](AbstractContext::Ptr context)
		{
			context->setProgram(programId);
		});

		for (unsigned int inputId = 0; inputId < program->inputs()->locations().size(); ++inputId)
		{
			auto type		= program->inputs()->types()[inputId];
			auto location	= program->inputs()->locations()[inputId];
			auto inputName	= program->inputs()->names()[inputId];
			auto name		= _inputNameToBindingName.count(inputName)
				? _inputNameToBindingName.find(inputName)->second
				: inputName;

			if (!_data->hasProperty(name))
				continue;

			if (type == ShaderProgramInputs::attribute)
			{
				auto vertexStream	= _data->get<VertexStream::Ptr>(name);
				auto attribute		= vertexStream->attribute(inputName);
				auto size			= std::get<1>(*attribute);
				auto offset			= std::get<2>(*attribute);
				auto vertexBuffer	= vertexStream->id();
				
				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setVertexBufferAt(
						location,
						vertexBuffer,
						size,
						vertexSize,
						offset
					);
				});
			}
			else if (type == ShaderProgramInputs::Type::float1)
			{
				auto floatValue = _data->get<float>(name);

				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setUniform(location, floatValue);
				});
			}
			else if (type == ShaderProgramInputs::Type::float2)
			{
				auto float2Value	= _data->get<std::shared_ptr<Vector2>>(name);
				auto x				= float2Value->x();
				auto y				= float2Value->y();

				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setUniform(location, x, y);
				});
			}
			else if (type == ShaderProgramInputs::Type::float3)
			{
				auto float3Value	= _data->get<std::shared_ptr<Vector3>>(name);
				auto x				= float3Value->x();
				auto y				= float3Value->y();
				auto z				= float3Value->z();

				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setUniform(location, x, y, z);
				});
			}
			else if (type == ShaderProgramInputs::Type::float4)
			{
				auto float4Value	= _data->get<std::shared_ptr<Vector4>>(name);
				auto x				= float4Value->x();
				auto y				= float4Value->y();
				auto z				= float4Value->z();
				auto w				= float4Value->w();

				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setUniform(location, x, y, z, w);
				});
			}
			else if (type == ShaderProgramInputs::Type::float16)
			{
				auto float16Ptr = &(_data->get<Matrix4x4::Ptr>(name)->data()[0]);

				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setUniformMatrix4x4(location, 1, true, float16Ptr);
				});
			}
			else if (type == ShaderProgramInputs::Type::sampler2d)
			{
				auto texture = _data->get<Texture::Ptr>(name)->id();

				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setTextureAt(numTextures, texture, location);
				});

				++numTextures;
			}
		}

		_func.push_back([=](AbstractContext::Ptr context)
		{
			auto count = numTextures;

			while (count < 8)
				context->setTextureAt(count++);
		});

		_func.push_back(drawTriangles);
	}

	
}

void
DrawCall::render(AbstractContext::Ptr context)
{
	for (auto& f : _func)
		f(context);
}
