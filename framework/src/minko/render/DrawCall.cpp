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

DrawCall::DrawCall(std::shared_ptr<data::Container>						data,
				   const std::unordered_map<std::string, std::string>&	attributeBindings,
				   const std::unordered_map<std::string, std::string>&	uniformBindings,
				   const std::unordered_map<std::string, std::string>&	stateBindings) :
	_data(data),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings)
{
}

void
DrawCall::bind()
{
    _func.clear();
    _propertyChangedSlots.clear();

	auto vertexSize		= getDataProperty<unsigned int>("geometry/vertex/size");
	auto indexStream	= getDataProperty<IndexStream::Ptr>("geometry/indices");
	auto indexBuffer	= indexStream->id();
	auto numIndices		= indexStream->data().size();
	auto drawTriangles	= [=](AbstractContext::Ptr context)
	{
		context->drawTriangles(indexBuffer, numIndices);
	};
	
	for (auto passId = 0; dataHasProperty("effect/pass" + std::to_string(passId)); ++passId)
	{
		auto program		= getDataProperty<Program::Ptr>("effect/pass" + std::to_string(passId));
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

			if (type == ProgramInputs::attribute)
			{
				auto name	= _attributeBindings.count(inputName)
					? _attributeBindings.at(inputName)
					: inputName;

				if (!dataHasProperty(name))
					continue;

				auto vertexStream	= getDataProperty<VertexStream::Ptr>(name);
				auto attribute		= vertexStream->attribute(inputName);
				auto size			= std::get<1>(*attribute);
				auto offset			= std::get<2>(*attribute);
				auto vertexBuffer	= vertexStream->id();
				
				_func.push_back([=](AbstractContext::Ptr context)
				{
					context->setVertexBufferAt(location, vertexBuffer, size, vertexSize, offset);
				});
			}
			else
			{
				auto name	= _uniformBindings.count(inputName)
					? _uniformBindings.at(inputName)
					: inputName;

				if (!dataHasProperty(name))
					continue;

				if (type == ProgramInputs::Type::float1)
				{
					auto floatValue = getDataProperty<float>(name);

					_func.push_back([=](AbstractContext::Ptr context)
					{
						context->setUniform(location, floatValue);
					});
				}
				else if (type == ProgramInputs::Type::float2)
				{
					auto float2Value	= getDataProperty<std::shared_ptr<Vector2>>(name);
					auto x				= float2Value->x();
					auto y				= float2Value->y();

					_func.push_back([=](AbstractContext::Ptr context)
					{
						context->setUniform(location, x, y);
					});
				}
				else if (type == ProgramInputs::Type::float3)
				{
					auto float3Value	= getDataProperty<std::shared_ptr<Vector3>>(name);
					auto x				= float3Value->x();
					auto y				= float3Value->y();
					auto z				= float3Value->z();

					_func.push_back([=](AbstractContext::Ptr context)
					{
						context->setUniform(location, x, y, z);
					});
				}
				else if (type == ProgramInputs::Type::float4)
				{
					auto float4Value	= getDataProperty<std::shared_ptr<Vector4>>(name);
					auto x				= float4Value->x();
					auto y				= float4Value->y();
					auto z				= float4Value->z();
					auto w				= float4Value->w();

					_func.push_back([=](AbstractContext::Ptr context)
					{
						context->setUniform(location, x, y, z, w);
					});
				}
				else if (type == ProgramInputs::Type::float16)
				{
					auto float16Ptr = &(getDataProperty<Matrix4x4::Ptr>(name)->data()[0]);

					_func.push_back([=](AbstractContext::Ptr context)
					{
						context->setUniformMatrix4x4(location, 1, true, float16Ptr);
					});
				}
				else if (type == ProgramInputs::Type::sampler2d)
				{
					auto texture = getDataProperty<Texture::Ptr>(name)->id();

					_func.push_back([=](AbstractContext::Ptr context)
					{
						context->setTextureAt(numTextures, texture, location);
					});

					++numTextures;
				}
			}
		}

		_func.push_back([=](AbstractContext::Ptr context)
		{
			auto count = numTextures;

			while (count < 8)
				context->setTextureAt(count++);
		});

		bindStates();

		_func.push_back(drawTriangles);
	}

	
}

void
DrawCall::bindStates()
{
	// blending state
	auto blending = _stateBindings.count("blending")
		? getDataProperty<Blending::Mode>(_stateBindings.at("blending"))
		: Blending::Mode::DEFAULT;
	_func.push_back([=](AbstractContext::Ptr context)
	{
		context->setBlendMode(blending);
	});
}

void
DrawCall::render(AbstractContext::Ptr context)
{
	for (auto& f : _func)
		f(context);
}

void
DrawCall::watchProperty(const std::string& propertyName)
{
    /*
    _propertyChangedSlots.push_back(_data->propertyChanged(propertyName)->connect(std::bind(
        &DrawCall::boundPropertyChangedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2
    )));
    */
}

void
DrawCall::boundPropertyChangedHandler(std::shared_ptr<data::Container>  data,
                                      const std::string&                propertyName)
{
    bind();
}

bool
DrawCall::dataHasProperty(const std::string& propertyName)
{
    watchProperty(propertyName);

    return _data->hasProperty(propertyName);
}
