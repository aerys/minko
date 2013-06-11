#include "DrawCall.hpp"

#include "minko/render/context/AbstractContext.hpp"
#include "minko/render/stream/VertexStream.hpp"
#include "minko/render/stream/IndexStream.hpp"
#include "minko/render/GLSLProgram.hpp"
#include "minko/render/stream/VertexFormat.hpp"
#include "minko/scene/data/DataBindings.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko::math;
using namespace minko::render;
using namespace minko::render::stream;

DrawCall::DrawCall(std::shared_ptr<DataBindings> bindings, const std::map<std::string, std::string>& inputNameToBindingName) :
	_bindings(bindings),
	_inputNameToBindingName(inputNameToBindingName)
{
	_vertexBuffer = bindings->getProperty<VertexStream<VertexFormat::XYZ>::ptr>("geometry/vertices/xyz")->buffer();
	_indexBuffer = bindings->getProperty<IndexStream::ptr>("geometry/indices")->buffer();
	_program = bindings->getProperty<GLSLProgram::ptr>("effect/pass0");
}

void
DrawCall::render(std::shared_ptr<AbstractContext> context)
{
	context->setProgram(_program->program());
	context->setVertexBufferAt(0, _vertexBuffer, "position");

	for (unsigned int inputId = 0; inputId < _program->inputs()->locations().size(); ++inputId)
	{
		auto type = _program->inputs()->types()[inputId];

		// FIXME: handle attributes
		if (type == ShaderProgramInputs::attribute || type == ShaderProgramInputs::unknown)
			continue;

		auto location = _program->inputs()->locations()[inputId];
		auto inputName = _program->inputs()->names()[inputId];
		auto name = _inputNameToBindingName.count(inputName)
			? _inputNameToBindingName.find(inputName)->second
			: inputName;

		if (type == ShaderProgramInputs::Type::float1)
			context->setUniform(location, _bindings->getProperty<float>(name));
		else if (type == ShaderProgramInputs::Type::float2)
		{
			auto float2Value = _bindings->getProperty<std::shared_ptr<Vector2>>(name);

			context->setUniform(location, float2Value->x(), float2Value->y());
		}
		else if (type == ShaderProgramInputs::Type::float3)
		{
			auto float3Value = _bindings->getProperty<std::shared_ptr<Vector3>>(name);

			context->setUniform(location, float3Value->x(), float3Value->y(), float3Value->z());
		}
		else if (type == ShaderProgramInputs::Type::float4)
		{
			auto float4Value = _bindings->getProperty<std::shared_ptr<Vector4>>(name);

			context->setUniform(location, float4Value->x(), float4Value->y(), float4Value->z(), float4Value->w());
		}
		else if (type == ShaderProgramInputs::Type::float16)
			context->setUniform(
				location,
				16,
				&(_bindings->getProperty<Matrix4x4::ptr>(name)->data()[0])
			);
	}

	context->drawTriangles(_indexBuffer, 12);
}
