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

#include "minko/render/AbstractContext.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/WrapMode.hpp"
#include "minko/render/TextureFilter.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Program.hpp"
#include "minko/render/States.hpp"
#include "minko/render/Priority.hpp"
#include "minko/data/Container.hpp"
#include "minko/math/Matrix4x4.hpp"

#include "DrawCallZSorter.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::math;
using namespace minko::render;

SamplerState DrawCall::_defaultSamplerState = SamplerState(WrapMode::CLAMP, TextureFilter::NEAREST, MipFilter::NONE);
/*static*/ const unsigned int	DrawCall::MAX_NUM_TEXTURES		= 8;
/*static*/ const unsigned int	DrawCall::MAX_NUM_VERTEXBUFFERS	= 8;

DrawCall::DrawCall(const data::BindingMap&	attributeBindings,
				   const data::BindingMap&	uniformBindings,
				   const data::BindingMap&	stateBindings,
                   States::Ptr              states) :
	_program(nullptr),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings),
    _states(states),
    _textureIds(MAX_NUM_TEXTURES, 0),
    _textureLocations(MAX_NUM_TEXTURES, -1),
    _textureWrapMode(MAX_NUM_TEXTURES, WrapMode::CLAMP),
    _textureFilters(MAX_NUM_TEXTURES, TextureFilter::NEAREST),
    _textureMipFilters(MAX_NUM_TEXTURES, MipFilter::NONE),
    _vertexBufferIds(MAX_NUM_VERTEXBUFFERS, 0),
    _vertexBufferLocations(MAX_NUM_VERTEXBUFFERS, -1),
    _vertexSizes(MAX_NUM_VERTEXBUFFERS, -1),
    _vertexAttributeSizes(MAX_NUM_VERTEXBUFFERS, -1),
    _vertexAttributeOffsets(MAX_NUM_VERTEXBUFFERS, -1),
	_target(nullptr),
	_referenceChangedSlots(),
	_indicesChanged(nullptr),
	_zsortNeeded(Signal<Ptr>::create()),
	_zSorter(nullptr)
{
}

void
DrawCall::initialize()
{
	_zSorter = DrawCallZSorter::create(shared_from_this());
}

void
DrawCall::configure(std::shared_ptr<Program>  program,
					ContainerPtr              data,
					ContainerPtr              rendererData,
                    ContainerPtr              rootData)
{
    _program = program;
    bind(data, rendererData, rootData);
}

void
DrawCall::bind(ContainerPtr data, ContainerPtr rendererData, ContainerPtr rootData)
{
	reset();

	bindProgramDefaultUniforms();

	_targetData		= data;
	_rendererData	= rendererData;
	_rootData		= rootData;

	bindIndexBuffer();
	bindProgramInputs();
	bindStates();

	_zSorter->initialize(data, rendererData, rootData);
}

void
DrawCall::bindProgramDefaultUniforms()
{
	if (_program == nullptr)
		return;

	for (auto& uniform : _program->uniformFloat())
		_uniformFloat[uniform.first] = uniform.second;
	for (auto& uniform : _program->uniformFloat2())
		_uniformFloat2[uniform.first] = uniform.second;
	for (auto& uniform : _program->uniformFloat3())
		_uniformFloat3[uniform.first] = uniform.second;
	for (auto& uniform : _program->uniformFloat4())
		_uniformFloat4[uniform.first] = uniform.second;
}

void
DrawCall::bindIndexBuffer()
{
	const std::string propertyName = "geometry[" + _variablesToValue["geometryId"] + "].indices";

	_indexBuffer	= -1;
	_numIndices		= 0;
	_indicesChanged	= nullptr;

	// Note: index buffer can only be held by the target node's data container!
	if (_targetData->hasProperty(propertyName))
	{
		auto indexBuffer	= _targetData->get<IndexBuffer::Ptr>(propertyName);
		_indexBuffer		= indexBuffer->id();
		_numIndices			= indexBuffer->numIndices();

		_indicesChanged		= indexBuffer->changed()->connect([&](IndexBuffer::Ptr indices){
			_indexBuffer	= indices->id();
			_numIndices		= indices->numIndices();
		});
	}

	if (_referenceChangedSlots.count(propertyName) == 0)
	{
		_referenceChangedSlots[propertyName].push_back(
			_targetData->propertyReferenceChanged(propertyName)->connect(std::bind(
				&DrawCall::bindIndexBuffer,
				shared_from_this())
			)
		);
	}
}

void
DrawCall::bindProgramInputs()
{
	if (_program == nullptr || _program->inputs() == nullptr)
		return;

	auto							programInputs		= _program->inputs();
	const std::vector<std::string>&	inputNames			= programInputs->names();
	unsigned int					numTextures			= 0;
    unsigned int					numVertexBuffers	= 0;
	
	unsigned int					vertexBufferIndex	= 0;
	unsigned int					textureIndex		= 0;
	
	for (unsigned int inputId = 0; inputId < inputNames.size(); ++inputId)
	{
		const std::string&	inputName	= inputNames[inputId];
		const auto			type		= programInputs->type(inputName);
		const int			location	= programInputs->location(inputName);

		switch (type)
		{
		case ProgramInputs::Type::attribute:
			{
				vertexBufferIndex	= bindVertexAttribute(inputName, location, vertexBufferIndex, true);
				break;
			}
	
		case ProgramInputs::Type::sampler2d:
		case ProgramInputs::Type::samplerCube:
			{
				auto& samplerState = _states->samplers().count(inputName)
					? _states->samplers().at(inputName)
					: _defaultSamplerState;

				textureIndex		= bindTextureSampler(inputName, location, textureIndex, samplerState, true);
				break;
			}
	
		default:
			{
				bindUniform(inputName, type, location);
				break;
			}
	
		case ProgramInputs::Type::unknown:
			break;
		}
	}
}

uint
DrawCall::bindVertexAttribute(const std::string&	inputName,
							  int					location,
							  uint					vertexBufferIndex,
							  bool					incrementIndex)
{
#ifdef DEBUG
	if (location < 0)
		throw std::invalid_argument("location");
	if (vertexBufferIndex >= MAX_NUM_VERTEXBUFFERS)
		throw std::invalid_argument("vertexBufferIndex");
#endif // DEBUG
	
	auto index = vertexBufferIndex;

	if (_attributeBindings.count(inputName))
	{
		auto propertyName		= formatPropertyName(std::get<0>(_attributeBindings.at(inputName)));
		const auto& container	= getDataContainer(std::get<1>(_attributeBindings.at(inputName)));

		if (container && container->hasProperty(propertyName))
		{
			auto vertexBuffer = container->get<VertexBuffer::Ptr>(propertyName);
			auto attributeName = propertyName.substr(propertyName.find_last_of('.') + 1);

#ifdef DEBUG
			if (!vertexBuffer->hasAttribute(attributeName))
				throw std::logic_error("missing required vertex attribute: " + attributeName);
#endif

			auto attribute	= vertexBuffer->attribute(attributeName);

			_vertexBufferIds		[index]	= vertexBuffer->id();
			_vertexBufferLocations	[index]	= location;
			_vertexAttributeSizes	[index]	= std::get<1>(*attribute);
			_vertexSizes			[index]	= vertexBuffer->vertexSize();
			_vertexAttributeOffsets	[index]	= std::get<2>(*attribute);

			if (incrementIndex)
				++index;
		}


		if (_referenceChangedSlots.count(propertyName) == 0)
		{
			auto that = shared_from_this();
			auto slot = container->propertyReferenceChanged(propertyName)->connect(
					[=](Container::Ptr, const std::string&)
					{
						that->bindVertexAttribute(inputName, location, vertexBufferIndex, false);
					}	
				);
			_referenceChangedSlots[propertyName].push_back(slot);
		}
	}

	return index;
}

uint
DrawCall::bindTextureSampler(const std::string&		inputName,
							 int					location,
							 uint					textureIndex,
   							 const SamplerState&	samplerState, 
							 bool					incrementIndex)
{
#ifdef DEBUG
	if (location < 0)
		throw std::invalid_argument("location");
	if (textureIndex >= MAX_NUM_TEXTURES)
		throw std::invalid_argument("textureIndex");
#endif // DEBUG

	auto index = textureIndex;

	if (_uniformBindings.count(inputName))
	{
		auto propertyName		= formatPropertyName(std::get<0>(_uniformBindings.at(inputName)));
		const auto& container	= getDataContainer(std::get<1>(_uniformBindings.at(inputName)));

		if (container && container->hasProperty(propertyName))
		{
			auto texture	= container->get<AbstractTexture::Ptr>(propertyName);

			_textureIds			[index] = texture->id();
			_textureLocations	[index] = location;
			_textureWrapMode	[index] = std::get<0>(samplerState);
			_textureFilters		[index] = std::get<1>(samplerState);
			_textureMipFilters	[index] = std::get<2>(samplerState);

			if (incrementIndex)
				++index;
		}

		if (_referenceChangedSlots.count(propertyName) == 0)			
		{
			auto that = shared_from_this();
			auto slot = container->propertyReferenceChanged(propertyName)->connect(
				[=](Container::Ptr, const std::string&)
				{
					that->bindTextureSampler(inputName, location, textureIndex, samplerState, false);
				}
			);
			_referenceChangedSlots[propertyName].push_back(slot);
		}
	}

	return index;
}

void
DrawCall::bindUniform(const std::string&	inputName,
					  ProgramInputs::Type	type,
					  int					location)
{
#ifdef DEBUG
	if (type == ProgramInputs::Type::sampler2d || type == ProgramInputs::Type::attribute)
		throw std::invalid_argument("type");
	if (location < 0)
		throw std::invalid_argument("location");
#endif // DEBUG

	bool		isArray		= false;
	auto		pos			= inputName.find_first_of('[');
	std::string	bindingName	= inputName;

	if (pos != std::string::npos)
	{
		bindingName = inputName.substr(0, pos);
		isArray = true;
	}
	
	if (_uniformBindings.count(bindingName))
	{	
		std::string	propertyName	= formatPropertyName(std::get<0>(_uniformBindings.at(bindingName)));
		auto&		source			= std::get<1>(_uniformBindings.at(bindingName));
		const auto&	container		= getDataContainer(source);

		if (container)
		{
			if (isArray)
				propertyName += inputName.substr(pos); // way to handle array of GLSL structs

			if (container->hasProperty(propertyName))
			{
				// This case corresponds to base types uniforms or individual members of an GLSL struct array.

				if (type == ProgramInputs::Type::float1)
					_uniformFloat[location]		= container->get<float>(propertyName);
				else if (type == ProgramInputs::Type::float2)
					_uniformFloat2[location]	= container->get<Vector2::Ptr>(propertyName);
				else if (type == ProgramInputs::Type::float3)
					_uniformFloat3[location]	= container->get<Vector3::Ptr>(propertyName);
				else if (type == ProgramInputs::Type::float4)
					_uniformFloat4[location]	= container->get<Vector4::Ptr>(propertyName);
				else if (type == ProgramInputs::Type::float16)
					_uniformFloat16[location]	= &(container->get<Matrix4x4::Ptr>(propertyName)->data()[0]);
				else if (type == ProgramInputs::Type::int1)
					_uniformInt[location]		= container->get<int>(propertyName);
				else if (type == ProgramInputs::Type::int2)
					_uniformInt2[location]		= container->get<Int2>(propertyName);
				else if (type == ProgramInputs::Type::int3)
					_uniformInt3[location]		= container->get<Int3>(propertyName); 
				else if (type == ProgramInputs::Type::int4)
					_uniformInt4[location]		= container->get<Int4>(propertyName);
				else
					throw std::logic_error("unsupported uniform type.");
			}
			else if (isArray)
			{
				// This case corresponds to continuous base type arrays that are stored in data providers as std::vector<float>.
				propertyName = formatPropertyName(std::get<0>(_uniformBindings.at(bindingName)));
				
				bindUniformArray(propertyName, container, type, location);
			}
		}

		if (_referenceChangedSlots.count(propertyName) == 0)			
		{
#if defined(EMSCRIPTEN)
			// See issue #1848 in Emscripten: https://github.com/kripken/emscripten/issues/1848
			auto that = shared_from_this();
			_referenceChangedSlots[propertyName].push_back(container->propertyReferenceChanged(propertyName)->connect([&, that, type, location](Container::Ptr, const std::string&) {
				that->bindUniform(inputName, type, location);
			}));
#else
			_referenceChangedSlots[propertyName].push_back(container->propertyReferenceChanged(propertyName)->connect(std::bind(
				&DrawCall::bindUniform, shared_from_this(), inputName, type, location
			)));
#endif		
		}
	}
}

void
DrawCall::bindUniformArray(const std::string&	propertyName,
						   Container::Ptr		container,
						   ProgramInputs::Type	type,
						   int					location)
{
	if (!container || !container->hasProperty(propertyName))
		return;

	if (type == ProgramInputs::Type::int1 ||
		type == ProgramInputs::Type::int2 ||
		type == ProgramInputs::Type::int3 ||
		type == ProgramInputs::Type::int4)
		bindIntegerUniformArray(propertyName, container, type, location);
	else
		bindFloatUniformArray(propertyName, container, type, location);
}

void
DrawCall::bindFloatUniformArray(const std::string&	propertyName,
								Container::Ptr		container,
								ProgramInputs::Type	type,
								int					location)
{
	if (!container->propertyHasType<UniformArrayPtr<float>>(propertyName, true))
		return;

	const auto& uniformArray = container->get<UniformArrayPtr<float>>(propertyName);
	if (uniformArray->first == 0 || uniformArray->second == nullptr)
		return;

	if (type == ProgramInputs::Type::float1)
		_uniformFloats[location] = uniformArray;
	else if (type == ProgramInputs::Type::float2)
		_uniformFloats2[location] = uniformArray;
	else if (type == ProgramInputs::Type::float3)
		_uniformFloats3[location] = uniformArray;
	else if (type == ProgramInputs::Type::float4)
		_uniformFloats4[location] = uniformArray;
	else if (type == ProgramInputs::Type::float16)
		_uniformFloats16[location] = uniformArray;
	else
		throw std::logic_error("unsupported uniform type.");
}

void
DrawCall::bindIntegerUniformArray(const std::string&	propertyName,
								  Container::Ptr		container,
							 	  ProgramInputs::Type	type,
								  int					location)
{
	if (!container->propertyHasType<UniformArrayPtr<int>>(propertyName, true))
		return;

	const auto& uniformArray = container->get<UniformArrayPtr<int>>(propertyName);
	if (uniformArray->first == 0 || uniformArray->second == nullptr)
		return;

	if (type == ProgramInputs::Type::int1)
		_uniformInts[location] = uniformArray;
	else if (type == ProgramInputs::Type::int2)
		_uniformInts2[location] = uniformArray;
	else if (type == ProgramInputs::Type::int3)
		_uniformInts3[location] = uniformArray;
	else if (type == ProgramInputs::Type::int4)
		_uniformInts4[location] = uniformArray;
	else
		throw std::logic_error("unsupported uniform type.");
}


void
DrawCall::reset()
{
	_target = nullptr;

	_uniformFloat.clear();
	_uniformFloat2.clear();
	_uniformFloat3.clear();
	_uniformFloat4.clear();
	_uniformFloat16.clear();

	_uniformInt.clear();
	_uniformInt2.clear();
	_uniformInt3.clear();
	_uniformInt4.clear();

	_uniformFloats.clear();
	_uniformFloats2.clear();
	_uniformFloats3.clear();
	_uniformFloats4.clear();
	_uniformFloats16.clear();

	_uniformInts.clear();
	_uniformInts2.clear();
	_uniformInts3.clear();
	_uniformInts4.clear();

	_textureIds			.clear();
	_textureLocations	.clear();
	_textureWrapMode	.clear();
	_textureFilters		.clear();
	_textureMipFilters	.clear();

	_textureIds			.resize(MAX_NUM_TEXTURES, 0);
	_textureLocations	.resize(MAX_NUM_TEXTURES, -1);
	_textureWrapMode	.resize(MAX_NUM_TEXTURES, WrapMode::CLAMP);
	_textureFilters		.resize(MAX_NUM_TEXTURES, TextureFilter::NEAREST);
	_textureMipFilters	.resize(MAX_NUM_TEXTURES, MipFilter::NONE);

	_vertexBufferIds			.clear();
	_vertexBufferLocations	.clear();
	_vertexSizes			.clear();
	_vertexAttributeSizes	.clear();
	_vertexAttributeOffsets	.clear();

	_vertexBufferIds			.resize(MAX_NUM_VERTEXBUFFERS, 0);
	_vertexBufferLocations	.resize(MAX_NUM_VERTEXBUFFERS, -1);
	_vertexSizes			.resize(MAX_NUM_VERTEXBUFFERS, -1);
	_vertexAttributeSizes	.resize(MAX_NUM_VERTEXBUFFERS, -1);
	_vertexAttributeOffsets	.resize(MAX_NUM_VERTEXBUFFERS, -1);

	_referenceChangedSlots.clear();
	_zSorter->clear();
}

void
DrawCall::bindStates()
{
	bindState<Blending::Mode>("blendMode", _states->blendingSourceFactor() | _states->blendingDestinationFactor(), _blendMode);
	bindState<bool>("colorMask", _states->colorMask(), _colorMask);
	bindState<bool>("depthMask", _states->depthMask(), _depthMask);
	bindState<CompareMode>("depthFunc", _states->depthFunc(), _depthFunc);
	bindState<TriangleCulling>("triangleCulling", _states->triangleCulling(), _triangleCulling);
	bindState<CompareMode>("stencilFunc", _states->stencilFunction(), _stencilFunc);
	bindState<int>("stencilRef", _states->stencilReference(), _stencilRef);
	bindState<uint>("stencilMask", _states->stencilMask(), _stencilMask);
	bindState<StencilOperation>("stencilFailOp", _states->stencilFailOperation(), _stencilFailOp);
	bindState<StencilOperation>("stencilZFailOp", _states->stencilDepthFailOperation(), _stencilZFailOp);
	bindState<StencilOperation>("stencilZPassOp", _states->stencilDepthPassOperation(), _stencilZPassOp);
	bindState<bool>("scissorTest", _states->scissorTest(), _scissorTest);
	bindState<int>("scissorBox.x", _states->scissorBox().x, _scissorBox.x);
	bindState<int>("scissorBox.y", _states->scissorBox().y, _scissorBox.y);
	bindState<int>("scissorBox.width", _states->scissorBox().width, _scissorBox.width);
	bindState<int>("scissorBox.height", _states->scissorBox().height, _scissorBox.height);
	bindState<float>("priority", _states->priority(), _priority);
	bindState<bool>("zSort", _states->zSorted(), _zsorted);
	bindState<AbstractTexture::Ptr>("target", _states->target(), _target);
}

void
DrawCall::render(const AbstractContext::Ptr& context, AbstractTexture::Ptr renderTarget)
{
	if (!renderTarget)
		renderTarget = _target;

    if (renderTarget)
    {
    	if (renderTarget->id() != context->renderTarget())
    	{
	        context->setRenderToTexture(renderTarget->id(), true);
	        context->clear();
	    }
    }
    else
        context->setRenderToBackBuffer();

    context->setProgram(_program->id());

	// float uniforms
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
        context->setUniform(uniformFloat16.first, 1, true, uniformFloat16.second);

	// integer uniforms
	for (auto& uniformInt : _uniformInt)
		context->setUniform(uniformInt.first, uniformInt.second);
	for (auto& uniformInt2 : _uniformInt2)
	{
		const auto& int2 = uniformInt2.second;
		context->setUniform(uniformInt2.first, std::get<0>(int2), std::get<1>(int2));
	}
	for (auto& uniformInt3 : _uniformInt3)
	{
		const auto& int3 = uniformInt3.second;
		context->setUniform(uniformInt3.first, std::get<0>(int3), std::get<1>(int3), std::get<2>(int3));
	}
	for (auto& uniformInt4 : _uniformInt4)
	{
		const auto& int4 = uniformInt4.second;
		context->setUniform(uniformInt4.first, std::get<0>(int4), std::get<1>(int4), std::get<2>(int4), std::get<3>(int4));
	}

	// arrays of float uniforms
	for (auto& uniformFloats : _uniformFloats)
		context->setUniforms	(uniformFloats.first,	uniformFloats.second->first,			uniformFloats.second->second);
	for (auto& uniformFloats2 : _uniformFloats2)
		context->setUniforms2	(uniformFloats2.first,	uniformFloats2.second->first,			uniformFloats2.second->second);
	for (auto& uniformFloats3 : _uniformFloats3)
		context->setUniforms3	(uniformFloats3.first,	uniformFloats3.second->first,			uniformFloats3.second->second);
	for (auto& uniformFloats4 : _uniformFloats4)
		context->setUniforms4	(uniformFloats4.first,	uniformFloats4.second->first,			uniformFloats4.second->second);
	for (auto& uniformFloats16 : _uniformFloats16)
		context->setUniform		(uniformFloats16.first,	uniformFloats16.second->first, false,	uniformFloats16.second->second);

	// arrays of integer uniforms
	for (auto& uniformInts: _uniformInts)
		context->setUniforms	(uniformInts.first,		uniformInts.second->first,			uniformInts.second->second);
	for (auto& uniformInts2 : _uniformInts2)
		context->setUniforms2	(uniformInts2.first,	uniformInts2.second->first,			uniformInts2.second->second);
	for (auto& uniformInts3 : _uniformInts3)
		context->setUniforms3	(uniformInts3.first,	uniformInts3.second->first,			uniformInts3.second->second);
	for (auto& uniformInts4 : _uniformInts4)
		context->setUniforms4	(uniformInts4.first,	uniformInts4.second->first,			uniformInts4.second->second);

	auto textureOffset = 0;
	for (auto textureLocationAndPtr : _program->textures())
		context->setTextureAt(
			textureOffset++, 
			textureLocationAndPtr.second->id(), 
			textureLocationAndPtr.first
		);

	for (uint i = 0; i < _textureIds.size() - textureOffset; ++i)
    {
        auto textureId = _textureIds[i];

        context->setTextureAt(
			textureOffset + i, 
			textureId, 
			_textureLocations[i]
		);
        if (textureId > 0)
            context->setSamplerStateAt(
				textureOffset + i,
				_textureWrapMode[i],
				_textureFilters[i],
				_textureMipFilters[i]
            );
    }

	// first, hand over to the context bound vertex attributes
	for (uint i = 0; i < _vertexBufferIds.size(); ++i)
	{
		auto vertexBufferId = _vertexBufferIds[i];
	
		if (vertexBufferId > 0 && 
			!_program->hasVertexBufferLocation(_vertexBufferLocations[i]))
			context->setVertexBufferAt(
			     _vertexBufferLocations[i],
			     vertexBufferId,
			     _vertexAttributeSizes[i],
			     _vertexSizes[i],
			     _vertexAttributeOffsets[i]
			);
	}
	// second, hand over explicitly user defined vertex attributes (possible replacement of )
	for (auto vertexBufferLocationAndPtr : _program->vertexBuffers())
	{
		const int	location		= vertexBufferLocationAndPtr.first;
		auto&		vertexBuffer	= vertexBufferLocationAndPtr.second;
	
		if (vertexBuffer->isReady())
		{
			assert(vertexBuffer->attributes().size() == 1);
	
			const auto& vertexAttribute = vertexBuffer->attributes().front();
	
			context->setVertexBufferAt(
				location,
				vertexBuffer->id(),
				std::get<1>(*vertexAttribute),
				vertexBuffer->vertexSize(),
				std::get<2>(*vertexAttribute)
			);
		}
	}
	
	context->setColorMask(_colorMask);
	context->setBlendMode(_blendMode);
	context->setDepthTest(_depthMask, _depthFunc);
	context->setStencilTest(_stencilFunc, _stencilRef, _stencilMask, _stencilFailOp, _stencilZFailOp, _stencilZPassOp);
	context->setScissorTest(_scissorTest, _scissorBox);
    context->setTriangleCulling(_triangleCulling);

	if (_program->indexBuffer() && _program->indexBuffer()->isReady())
		context->drawTriangles(_program->indexBuffer()->id(), _program->indexBuffer()->data().size() / 3);
	else if (_indexBuffer != -1)
		context->drawTriangles(_indexBuffer, _numIndices / 3);
}

Container::Ptr
DrawCall::getDataContainer(const data::BindingSource& source) const
{
	if (source == data::BindingSource::TARGET)
		return _targetData;
	else if (source == data::BindingSource::RENDERER)
		return _rendererData;
	else if (source == data::BindingSource::ROOT)
		return _rootData;

	return nullptr;
}

Vector3::Ptr
DrawCall::getEyeSpacePosition(Vector3::Ptr output) 
{
	return _zSorter->getEyeSpacePosition(output);
}
