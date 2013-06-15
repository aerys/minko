#include "DrawCall.hpp"

#include "minko/render/context/AbstractContext.hpp"
#include "minko/render/stream/VertexStream.hpp"
#include "minko/render/stream/IndexStream.hpp"
#include "minko/render/GLSLProgram.hpp"
#include "minko/render/stream/VertexFormat.hpp"
#include "minko/scene/data/DataBindings.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/stream/VertexAttribute.hpp"

using namespace minko::math;
using namespace minko::render;
using namespace minko::render::stream;

DrawCall::DrawCall(std::shared_ptr<DataBindings> bindings, const std::unordered_map<std::string, std::string>& inputNameToBindingName) :
	_bindings(bindings),
	_inputNameToBindingName(inputNameToBindingName)
{
	bind(bindings);
}

void
DrawCall::bind(std::shared_ptr<DataBindings> bindings)
{
	for (auto passId = 0; bindings->hasProperty("effect/pass" + std::to_string(passId)); ++passId)
	{
		auto program = bindings->getProperty<GLSLProgram::ptr>("effect/pass" + std::to_string(passId));

		_func.push_back([program](std::shared_ptr<AbstractContext> context)
		{
			context->setProgram(program->program());
		});

		for (unsigned int inputId = 0; inputId < program->inputs()->locations().size(); ++inputId)
		{
			auto type		= program->inputs()->types()[inputId];
			auto location	= program->inputs()->locations()[inputId];
			auto inputName	= program->inputs()->names()[inputId];
			auto name		= _inputNameToBindingName.count(inputName)
				? _inputNameToBindingName.find(inputName)->second
				: inputName;

			if (type == ShaderProgramInputs::attribute)
			{
				auto vertexStream	= _bindings->getProperty<VertexStream::ptr>(name);
				auto attribute		= vertexStream->attribute(inputName);

				_func.push_back([location, vertexStream, attribute](std::shared_ptr<AbstractContext> context)
				{
					context->setVertexBufferAt(location, vertexStream->buffer(), attribute->size(), attribute->offset());
				});
			}
			else if (type == ShaderProgramInputs::Type::float1)
			{
				auto floatValue = _bindings->getProperty<float>(name);

				_func.push_back([location, floatValue](std::shared_ptr<AbstractContext> context)
				{
					context->setUniform(location, floatValue);
				});
			}
			else if (type == ShaderProgramInputs::Type::float2)
			{
				auto float2Value = _bindings->getProperty<std::shared_ptr<Vector2>>(name);

				_func.push_back([location, float2Value](std::shared_ptr<AbstractContext> context)
				{
					context->setUniform(location, float2Value->x(), float2Value->y());
				});
			}
			else if (type == ShaderProgramInputs::Type::float3)
			{
				auto float3Value = _bindings->getProperty<std::shared_ptr<Vector3>>(name);

				_func.push_back([location, float3Value](std::shared_ptr<AbstractContext> context)
				{
					context->setUniform(location, float3Value->x(), float3Value->y(), float3Value->z());
				});
			}
			else if (type == ShaderProgramInputs::Type::float4)
			{
				auto float4Value = _bindings->getProperty<std::shared_ptr<Vector4>>(name);

				_func.push_back([location, float4Value](std::shared_ptr<AbstractContext> context)
				{
					context->setUniform(location, float4Value->x(), float4Value->y(), float4Value->z(), float4Value->w());
				});
			}
			else if (type == ShaderProgramInputs::Type::float16)
			{
				auto float16Ptr = &(_bindings->getProperty<Matrix4x4::ptr>(name)->data()[0]);

				_func.push_back([location, float16Ptr](std::shared_ptr<AbstractContext> context)
				{
					context->setUniformMatrix4x4(location, 1, true, float16Ptr);
				});
			}
		}
	}

	auto indexStream = bindings->getProperty<IndexStream::ptr>("geometry/indices");

	_func.push_back([indexStream](std::shared_ptr<AbstractContext> context)
	{
		context->drawTriangles(indexStream->buffer(), indexStream->data().size());
	});
}

void
DrawCall::render(std::shared_ptr<AbstractContext> context)
{
	for (auto f : _func)
		f(context);
}
