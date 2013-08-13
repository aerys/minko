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
#include "minko/render/CompareMode.hpp"
#include "minko/render/WrapMode.hpp"
#include "minko/render/TextureFilter.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/Program.hpp"
#include "minko/render/States.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/Binding.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::render;
using namespace minko::render;

SamplerState DrawCall::_defaultSamplerState = SamplerState(WrapMode::CLAMP, TextureFilter::NEAREST, MipFilter::NONE);

DrawCall::DrawCall(Program::Ptr				program,
                   const data::BindingMap&	attributeBindings,
				   const data::BindingMap&	uniformBindings,
				   const data::BindingMap&	stateBindings,
                   States::Ptr              states) :
	_program(program),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings),
    _states(states),
    _textures(8, -1),
    _textureLocations(8, -1),
    _textureWrapMode(8, WrapMode::CLAMP),
    _textureFilters(8, TextureFilter::NEAREST),
    _textureMipFilters(8, MipFilter::NONE),
    _vertexBuffers(8, -1),
    _vertexBufferLocations(8, -1),
    _vertexSizes(8, -1),
    _vertexAttributeSizes(8, -1),
    _vertexAttributeOffsets(8, -1)
{
}

void
DrawCall::bind(ContainerPtr data, ContainerPtr rootData)
{
	_data = data;
	_rootData = rootData;
    _propertyChangedSlots.clear();

	auto indexBuffer	= getDataProperty<IndexBuffer::Ptr>("geometry.indices");

    _indexBuffer = indexBuffer->id();
    _numIndices = indexBuffer->data().size();

	auto numTextures	    = 0;
    auto numVertexBuffers   = 0;
    auto programInputs      = _program->inputs();

    for (unsigned int inputId = 0; inputId < programInputs->locations().size(); ++inputId)
	{
		auto type		= programInputs->types()[inputId];
		auto location	= programInputs->locations()[inputId];
		auto inputName	= programInputs->names()[inputId];

		if (type == ProgramInputs::Type::attribute)
		{
			auto& name	= _attributeBindings.count(inputName)
				? _attributeBindings.at(inputName)->propertyName()
				: inputName;

			if (!dataHasProperty(name))
				continue;

			auto vertexBuffer	= getDataProperty<VertexBuffer::Ptr>(name);
			auto attribute		= vertexBuffer->attribute(inputName);

            _vertexBuffers[numVertexBuffers] = vertexBuffer->id();
            _vertexBufferLocations[numVertexBuffers] = location;
            _vertexAttributeSizes[numVertexBuffers] = std::get<1>(*attribute);
            _vertexSizes[numVertexBuffers] = vertexBuffer->vertexSize();
            _vertexAttributeOffsets[numVertexBuffers] = std::get<2>(*attribute);
			
            ++numVertexBuffers;
		}
		else
		{
			auto& name	= _uniformBindings.count(inputName)
				? _uniformBindings.at(inputName)->propertyName()
				: inputName;

			if (!dataHasProperty(name))
				continue;

			if (type == ProgramInputs::Type::float1)
                _uniformFloat[location] = getDataProperty<float>(name);
			else if (type == ProgramInputs::Type::float2)
                _uniformFloat2[location] = getDataProperty<std::shared_ptr<Vector2>>(name);
			else if (type == ProgramInputs::Type::float3)
                _uniformFloat3[location] = getDataProperty<std::shared_ptr<Vector3>>(name);
			else if (type == ProgramInputs::Type::float4)
                _uniformFloat4[location] = getDataProperty<std::shared_ptr<Vector4>>(name);
			else if (type == ProgramInputs::Type::float16)
                _uniformFloat16[location] = &(getDataProperty<Matrix4x4::Ptr>(name)->data()[0]);
			else if (type == ProgramInputs::Type::sampler2d)
			{
				auto texture        = getDataProperty<Texture::Ptr>(name)->id();
                auto& samplerState  = _states->samplers().count(inputName)
                    ? _states->samplers().at(inputName)
                    : _defaultSamplerState;

                _textures[numTextures] = texture;
                _textureLocations[numTextures] = location;
                _textureWrapMode[numTextures] = std::get<0>(samplerState);
                _textureFilters[numTextures] = std::get<1>(samplerState);
                _textureMipFilters[numTextures] = std::get<2>(samplerState);

				++numTextures;
			}
		}
	}

	bindStates();
}

void
DrawCall::bindStates()
{
	_blendMode = getDataProperty<Blending::Mode>(
        _stateBindings.count("blendMode") ? _stateBindings.at("blendMode")->propertyName() : "blendMode",
        _states->blendingSourceFactor() | _states->blendingDestinationFactor()
    );

	_depthMask = getDataProperty<bool>(
		_stateBindings.count("depthMask") ? _stateBindings.at("depthMask")->propertyName() : "depthMask",
        _states->depthMask()
	);
	_depthFunc = getDataProperty<CompareMode>(
		_stateBindings.count("depthFunc") ? _stateBindings.at("depthFunc")->propertyName() : "depthFunc",
        _states->depthFun()
	);

    _triangleCulling = getDataProperty<TriangleCulling>(
		_stateBindings.count("triangleCulling")
            ? _stateBindings.at("triangleCulling")->propertyName()
            : "triangleCulling",
        _states->triangleCulling()
	);

    _target = getDataProperty<Texture::Ptr>(
    	_stateBindings.count("target") ? _stateBindings.at("target")->propertyName() : "target",
        _states->target()
    );
	
    if (_target && !_target->isReady())
        _target->upload();
}

void
DrawCall::render(AbstractContext::Ptr context)
{
    if (_target)
        context->setRenderToTexture(_target->id(), true);
    else
        context->setRenderToBackBuffer();

    context->setProgram(_program->id());

    for (auto& uniformFloat : _uniformFloat)
        context->setUniform(uniformFloat.first, uniformFloat.second);
    for (auto& uniformFloat2 : _uniformFloat2)
    {
        auto& float2 = uniformFloat2.second;

        context->setUniform(uniformFloat2.first, float2->x(), float2->y());
    }
    for (auto& uniformFloat3 : _uniformFloat3)
    {
        auto& float3 = uniformFloat3.second;

        context->setUniform(uniformFloat3.first, float3->x(), float3->y(), float3->z());
    }
    for (auto& uniformFloat4 : _uniformFloat4)
    {
        auto& float4 = uniformFloat4.second;

        context->setUniform(uniformFloat4.first, float4->x(), float4->y(), float4->z(), float4->w());
    }
    for (auto& uniformFloat16 : _uniformFloat16)
        context->setUniformMatrix4x4(uniformFloat16.first, 1, true, uniformFloat16.second);

    for (uint textureId = 0; textureId < _textures.size(); ++textureId)
    {
        auto texture = _textures[textureId];

        context->setTextureAt(textureId, texture, _textureLocations[textureId]);
        if (texture > 0)
            context->setSamplerStateAt(
                textureId, _textureWrapMode[textureId], _textureFilters[textureId], _textureMipFilters[textureId]
            );
    }

    for (uint vertexBufferId = 0; vertexBufferId < _vertexBuffers.size(); ++vertexBufferId)
    {
        auto vertexBuffer = _vertexBuffers[vertexBufferId];

        if (vertexBuffer > 0)
            context->setVertexBufferAt(
                _vertexBufferLocations[vertexBufferId],
                vertexBuffer,
                _vertexAttributeSizes[vertexBufferId],
                _vertexSizes[vertexBufferId],
                _vertexAttributeOffsets[vertexBufferId]
            );
    }

	context->setBlendMode(_blendMode);
	context->setDepthTest(_depthMask, _depthFunc);
    context->setTriangleCulling(_triangleCulling);

    context->drawTriangles(_indexBuffer, _numIndices / 3);
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
    //bind();
}

bool
DrawCall::dataHasProperty(const std::string& propertyName)
{
    //watchProperty(propertyName);

    return _data->hasProperty(propertyName) || _rootData->hasProperty(propertyName);
}
