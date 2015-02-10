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

#include "minko/render/DrawCall.hpp"

#include "minko/render/DrawCallZSorter.hpp"
#include "minko/data/Store.hpp"
#include "minko/log/Logger.hpp"

// #include <regex>

using namespace minko;
using namespace minko::render;

const unsigned int	DrawCall::MAX_NUM_TEXTURES		            = 8;
const unsigned int	DrawCall::MAX_NUM_VERTEXBUFFERS	            = 8;

DrawCall::DrawCall(uint                   batchId,
                   std::shared_ptr<Pass>  pass,
                   const EffectVariables& variables,
                   data::Store&           rootData,
                   data::Store&           rendererData,
                   data::Store&           targetData) :
    _batchId(batchId),
    _pass(pass),
    _variables(variables),
    _rootData(rootData),
    _rendererData(rendererData),
    _targetData(targetData),
    _zSorter(DrawCallZSorter::create(this)),
    _zSortNeeded(Signal<DrawCall*>::create()),
    _target(nullptr),
    _indexBuffer(nullptr),
    _firstIndex(nullptr),
    _numIndices(nullptr)
{
    _zSorter->initialize(_targetData, _rendererData, _rootData);
}

data::Store&
DrawCall::getStore(data::Binding::Source source)
{
    switch (source)
    {
        case data::Binding::Source::ROOT:
            return _rootData;
        case data::Binding::Source::RENDERER:
            return _rendererData;
        case data::Binding::Source::TARGET:
            return _targetData;
    }

    throw;
}

void
DrawCall::reset()
{
    _program = nullptr;
    _indexBuffer = 0;
    _firstIndex = 0;
    _numIndices = 0;
    _uniformFloat.clear();
    _uniformInt.clear();
    _uniformBool.clear();
    _samplers.clear();
    _attributes.clear();
}

void
DrawCall::bind(std::shared_ptr<Program> program)
{
    reset();

    _program = program;

    // bindIndexBuffer();
    // bindStates();
    //bindUniforms();
    // bindAttributes();
}

void
DrawCall::bindAttribute(ConstAttrInputRef                                       input,
                        const std::unordered_map<std::string, data::Binding>&   attributeBindings,
                        const data::Store&                                      defaultValues)
{
    data::ResolvedBinding* binding = resolveBinding(input.name, attributeBindings);

    if (binding == nullptr)
    {
        if (!defaultValues.hasProperty(input.name))
        {
            auto it = std::find(_program->setAttributeNames().begin(), _program->setAttributeNames().end(), input.name);

            if (it == _program->setAttributeNames().end())
            {
                throw std::runtime_error(
                    "Program \"" + _program->name() + "\": the attribute \"" + input.name
                    + "\" is not bound, has not been set and no default value was provided."
                );
            }

            setAttributeValueFromStore(input, input.name, defaultValues);
        }
    }
    else
    {
#ifdef DEBUG
        const auto& setAttributes = _program->setAttributeNames();

        if (std::find(setAttributes.begin(), setAttributes.end(), input.name) != setAttributes.end())
        {
            LOG_WARNING(
                "Program \"" + _program->name() + "\", vertex attribute \""
                + input.name + "\" set manually but overriden by a binding to the \""
                + binding->propertyName + "\" property."
            );
        }
#endif

        if (!binding->store.hasProperty(binding->propertyName))
        {
            if (!defaultValues.hasProperty(input.name))
            {
                throw std::runtime_error(
                    "Program \"" + _program->name() + "\": the attribute \""
                    + input.name + "\" is bound to the \"" + binding->propertyName
                    + "\" property but it's not defined and no default value was provided."
                );
            }

            setAttributeValueFromStore(input, input.name, defaultValues);
        }
        else
            setAttributeValueFromStore(input, binding->propertyName, binding->store);

        delete binding;
    }
}

void
DrawCall::setAttributeValueFromStore(const ProgramInputs::AttributeInput& input,
                                     const std::string&                   propertyName,
                                     const data::Store&                   store)
{
    const auto* attr = store.getUnsafePointer<VertexAttribute>(propertyName);

    _attributes.push_back({
        input.location,
        attr->resourceId,
        attr->size,
        attr->vertexSize,
        attr->offset
    });
}

data::ResolvedBinding*
DrawCall::bindUniform(ConstUniformInputRef                                      input,
                      const std::unordered_map<std::string, data::Binding>&     uniformBindings,
                      const data::Store&                                        defaultValues)
{
    data::ResolvedBinding* binding = resolveBinding(input.name, uniformBindings);

    if (binding == nullptr)
    {
        if (!defaultValues.hasProperty(input.name))
        {
            auto it = std::find(_program->setUniformNames().begin(), _program->setUniformNames().end(), input.name);

            if (it == _program->setUniformNames().end())
            {
                throw std::runtime_error(
                    "Program \"" + _program->name() + "\": the uniform \"" + input.name
                    + "\" is not bound, has not been set and no default value was provided."
                );
            }
        }

        setUniformValueFromStore(input, input.name, defaultValues);
    }
    else
    {
        if (!binding->store.hasProperty(binding->propertyName))
        {
            if (!defaultValues.hasProperty(input.name))
            {
                throw std::runtime_error(
                    "Program \"" + _program->name() + "\": the uniform \""
                    + input.name + "\" is bound to the \"" + binding->propertyName
                    + "\" property but it's not defined and no default value was provided."
                );
            }
            else
                setUniformValueFromStore(input, input.name, defaultValues);
        }
        else
            setUniformValueFromStore(input, binding->propertyName, binding->store);
    }

    return binding;
}

std::array<data::ResolvedBinding*, 3>
DrawCall::bindSamplerStates(ConstUniformInputRef                                    input,
                            const std::unordered_map<std::string, data::Binding>&   uniformBindings,
                            const data::Store&                                      defaultValues)
{
    auto wrapModeBinding = bindSamplerState(input, uniformBindings, defaultValues, SamplerStates::PROPERTY_WRAP_MODE);
    auto textureFilterBinding = bindSamplerState(input, uniformBindings, defaultValues, SamplerStates::PROPERTY_TEXTURE_FILTER);
    auto mipFilterBinding = bindSamplerState(input, uniformBindings, defaultValues, SamplerStates::PROPERTY_MIP_FILTER);

    SamplerStatesResolveBindings samplerStatesResolveBindings = {
        wrapModeBinding,
        textureFilterBinding,
        mipFilterBinding
    };

    return samplerStatesResolveBindings;
}

data::ResolvedBinding*
DrawCall::bindSamplerState(ConstUniformInputRef                                     input,
                           const std::unordered_map<std::string, data::Binding>&    uniformBindings,
                           const data::Store&                                       defaultValues,
                           const std::string&                                       samplerStateProperty)
{
    if (samplerStateProperty == SamplerStates::PROPERTY_WRAP_MODE ||
        samplerStateProperty == SamplerStates::PROPERTY_TEXTURE_FILTER ||
        samplerStateProperty == SamplerStates::PROPERTY_MIP_FILTER)
    {
        auto samplerStateUniformName = SamplerStates::uniformNameToSamplerStateName(
            input.name,
            samplerStateProperty
        );

        auto binding = resolveBinding(samplerStateUniformName, uniformBindings);

        if (binding == nullptr)
        {
            setSamplerStateValueFromStore(input, samplerStateUniformName, defaultValues, samplerStateProperty);
        }
        else
        {
            if (!binding->store.hasProperty(binding->propertyName))
                setSamplerStateValueFromStore(input, samplerStateUniformName, defaultValues, samplerStateProperty);
            else
                setSamplerStateValueFromStore(input, binding->propertyName, binding->store, samplerStateProperty);
        }

        return binding;
    }

    return nullptr;
}

void
DrawCall::setUniformValueFromStore(const ProgramInputs::UniformInput&   input,
                                   const std::string&                   propertyName,
                                   const data::Store&                   store)
{
    switch (input.type)
    {
        case ProgramInputs::Type::bool1:
            setUniformValue(_uniformBool, input.location, 1, input.size, store.getPointer<int>(propertyName));
            break;
        case ProgramInputs::Type::bool2:
            setUniformValue(_uniformBool, input.location, 2, input.size, math::value_ptr(store.get<math::ivec2>(propertyName)));
            break;
        case ProgramInputs::Type::bool3:
            setUniformValue(_uniformBool, input.location, 3, input.size, math::value_ptr(store.get<math::ivec3>(propertyName)));
            break;
        case ProgramInputs::Type::bool4:
            setUniformValue(_uniformBool, input.location, 4, input.size, math::value_ptr(store.get<math::ivec4>(propertyName)));
            break;
        case ProgramInputs::Type::int1:
            setUniformValue(_uniformInt, input.location, 1, input.size, store.getPointer<int>(propertyName));
            break;
        case ProgramInputs::Type::int2:
            setUniformValue(_uniformInt, input.location, 2, input.size, math::value_ptr(store.get<math::ivec2>(propertyName)));
            break;
        case ProgramInputs::Type::int3:
            setUniformValue(_uniformInt, input.location, 3, input.size, math::value_ptr(store.get<math::ivec3>(propertyName)));
            break;
        case ProgramInputs::Type::int4:
            setUniformValue(_uniformInt, input.location, 4, input.size, math::value_ptr(store.get<math::ivec4>(propertyName)));
            break;
        case ProgramInputs::Type::float1:
            setUniformValue(_uniformFloat, input.location, 1, input.size, store.getPointer<float>(propertyName));
            break;
        case ProgramInputs::Type::float2:
            setUniformValue(_uniformFloat, input.location, 2, input.size, math::value_ptr(store.get<math::vec2>(propertyName)));
            break;
        case ProgramInputs::Type::float3:
            setUniformValue(_uniformFloat, input.location, 3, input.size, math::value_ptr(store.get<math::vec3>(propertyName)));
            break;
        case ProgramInputs::Type::float4:
            setUniformValue(_uniformFloat, input.location, 4, input.size, math::value_ptr(store.get<math::vec4>(propertyName)));
            break;
        case ProgramInputs::Type::float16:
            setUniformValue(_uniformFloat, input.location, 16, input.size, math::value_ptr(store.get<math::mat4>(propertyName)));
            break;
        case ProgramInputs::Type::sampler2d:
            _samplers.push_back({
                static_cast<uint>(_program->setTextureNames().size() + _samplers.size()),
                store.getPointer<TextureSampler>(propertyName)->id,
                input.location
            });
        break;
        case ProgramInputs::Type::float9:
        case ProgramInputs::Type::unknown:
        case ProgramInputs::Type::samplerCube:
            throw std::runtime_error("unsupported program input type: " + ProgramInputs::typeToString(input.type));
        break;
    }
}

void
DrawCall::setSamplerStateValueFromStore(const ProgramInputs::UniformInput&  input,
                                        const std::string&                  propertyName,
                                        const data::Store&                  store,
                                        const std::string&                  samplerStateProperty)
{
    auto it = std::find_if(_samplers.begin(), _samplers.end(),
        [&](SamplerValue sampler) -> bool
        {
            return sampler.location == input.location;
        }
    );

    if (it != _samplers.end())
    {
        auto& sampler = *it;

        if (samplerStateProperty == SamplerStates::PROPERTY_WRAP_MODE)
        {
            if (store.hasProperty(propertyName))
                sampler.wrapMode = store.getUnsafePointer<WrapMode>(propertyName);
            else
                sampler.wrapMode = &SamplerStates::DEFAULT_WRAP_MODE;
        }
        else if (samplerStateProperty == SamplerStates::PROPERTY_TEXTURE_FILTER)
        {
            if (store.hasProperty(propertyName))
                sampler.textureFilter = store.getUnsafePointer<TextureFilter>(propertyName);
            else
                sampler.textureFilter = &SamplerStates::DEFAULT_TEXTURE_FILTER;
        }
        else if (samplerStateProperty == SamplerStates::PROPERTY_MIP_FILTER)
        {
            if (store.hasProperty(propertyName))
                sampler.mipFilter = store.getUnsafePointer<MipFilter>(propertyName);
            else
                sampler.mipFilter = &SamplerStates::DEFAULT_MIP_FILTER;
        }
    }
}

void
DrawCall::bindIndexBuffer()
{
    auto indexBufferProperty = data::Store::getActualPropertyName(_variables, "geometry[${geometryUuid}].indices");
    if (_targetData.hasProperty(indexBufferProperty))
        _indexBuffer = const_cast<int*>(_targetData.getPointer<int>(indexBufferProperty));

    auto firstIndexProperty = data::Store::getActualPropertyName(_variables, "geometry[${geometryUuid}].firstIndex");
    if (_targetData.hasProperty(firstIndexProperty))
        _firstIndex = const_cast<uint*>(_targetData.getPointer<uint>(firstIndexProperty));

    auto numIndicesProperty = data::Store::getActualPropertyName(_variables, "geometry[${geometryUuid}].numIndices");
    if (_targetData.hasProperty(numIndicesProperty))
        _numIndices = const_cast<uint*>(_targetData.getPointer<uint>(numIndicesProperty));
}

void
DrawCall::bindStates(const std::unordered_map<std::string, data::Binding>&  stateBindings,
                     const data::Store&                                     defaultValues)
{
    _priority = bindState<float>(States::PROPERTY_PRIORITY, stateBindings, defaultValues);
    _zSorted = bindState<bool>(States::PROPERTY_ZSORTED, stateBindings, defaultValues);
    _blendingSourceFactor = bindState<Blending::Source>(States::PROPERTY_BLENDING_SOURCE, stateBindings, defaultValues);
    _blendingDestinationFactor = bindState<Blending::Destination>(States::PROPERTY_BLENDING_DESTINATION, stateBindings, defaultValues);
    _colorMask = bindState<bool>(States::PROPERTY_COLOR_MASK, stateBindings, defaultValues);
    _depthMask = bindState<bool>(States::PROPERTY_DEPTH_MASK, stateBindings, defaultValues);
    _depthFunc = bindState<CompareMode>(States::PROPERTY_DEPTH_FUNCTION, stateBindings, defaultValues);
    _triangleCulling = bindState<TriangleCulling>(States::PROPERTY_TRIANGLE_CULLING, stateBindings, defaultValues);
    _stencilFunction = bindState<CompareMode>(States::PROPERTY_STENCIL_FUNCTION, stateBindings, defaultValues);
    _stencilReference = bindState<int>(States::PROPERTY_STENCIL_REFERENCE, stateBindings, defaultValues);
    _stencilMask = bindState<uint>(States::PROPERTY_STENCIL_MASK, stateBindings, defaultValues);
    _stencilFailOp = bindState<StencilOperation>(States::PROPERTY_STENCIL_FAIL_OP, stateBindings, defaultValues);
    _stencilZFailOp = bindState<StencilOperation>(States::PROPERTY_STENCIL_ZFAIL_OP, stateBindings, defaultValues);
    _stencilZPassOp = bindState<StencilOperation>(States::PROPERTY_STENCIL_ZPASS_OP, stateBindings, defaultValues);
    _scissorTest = bindState<bool>(States::PROPERTY_SCISSOR_TEST, stateBindings, defaultValues);
    _scissorBox = bindState<math::ivec4>(States::PROPERTY_SCISSOR_BOX, stateBindings, defaultValues);
    _target = bindState<TextureSampler>(States::PROPERTY_TARGET, stateBindings, defaultValues);
}

void
DrawCall::render(AbstractContext::Ptr   context,
                 AbstractTexture::Ptr   renderTarget,
                 const math::ivec4&     viewport,
                 uint                   clearColor) const
{
    context->setProgram(_program->id());

    auto hasOwnTarget = _target && _target->id;
    auto renderTargetId = hasOwnTarget
        ? *_target->id
        : renderTarget ? renderTarget->id() : 0;
    bool targetChanged = false;

    if (renderTargetId)
    {
        if (renderTargetId != context->renderTarget())
        {
            context->setRenderToTexture(renderTargetId, true);

            if (hasOwnTarget)
                context->clear(
                    ((clearColor >> 24) & 0xff) / 255.f,
                    ((clearColor >> 16) & 0xff) / 255.f,
                    ((clearColor >> 8) & 0xff) / 255.f,
                    (clearColor & 0xff) / 255.f
                );

            targetChanged = true;
        }
    }
    else
        context->setRenderToBackBuffer();

    if (targetChanged && !hasOwnTarget && viewport.z >= 0 && viewport.w >= 0)
        context->configureViewport(viewport.x, viewport.y, viewport.z, viewport.w);

    for (const auto& u : _uniformBool)
    {
        if (u.size == 1)
            context->setUniformInt(u.location, u.count, u.data);
        else if (u.size == 2)
            context->setUniformInt2(u.location, u.count, u.data);
        else if (u.size == 3)
            context->setUniformInt3(u.location, u.count, u.data);
        else if (u.size == 4)
            context->setUniformInt4(u.location, u.count, u.data);
    }

    for (const auto& u : _uniformInt)
    {
        if (u.size == 1)
            context->setUniformInt(u.location, u.count, u.data);
        else if (u.size == 2)
            context->setUniformInt2(u.location, u.count, u.data);
        else if (u.size == 3)
            context->setUniformInt3(u.location, u.count, u.data);
        else if (u.size == 4)
            context->setUniformInt4(u.location, u.count, u.data);
    }

    for (const auto& u : _uniformFloat)
    {
        if (u.size == 1)
            context->setUniformFloat(u.location, u.count, u.data);
        else if (u.size == 2)
            context->setUniformFloat2(u.location, u.count, u.data);
        else if (u.size == 3)
            context->setUniformFloat3(u.location, u.count, u.data);
        else if (u.size == 4)
            context->setUniformFloat4(u.location, u.count, u.data);
        else if (u.size == 16)
            context->setUniformMatrix4x4(u.location, u.count, u.data);
    }

    for (const auto& s : _samplers)
    {
        context->setTextureAt(s.position, *s.resourceId, s.location);
        context->setSamplerStateAt(s.position, *s.wrapMode, *s.textureFilter, *s.mipFilter);
    }

    // for (auto numSamplers = _samplers.size(); numSamplers < MAX_NUM_TEXTURES; ++numSamplers)
    //     context->setTextureAt(numSamplers, -1, -1);

    for (const auto& a : _attributes)
        context->setVertexBufferAt(a.location, *a.resourceId, a.size, *a.stride, a.offset);
    // for (auto numAttributes = _attributes.size(); numAttributes < MAX_NUM_VERTEXBUFFERS; ++numAttributes)
    //     context->setVertexBufferAt(numAttributes, -1, 0, 0, 0);

    context->setColorMask(*_colorMask);
    context->setBlendingMode(*_blendingSourceFactor, *_blendingDestinationFactor);
    context->setDepthTest(*_depthMask, *_depthFunc);
    context->setStencilTest(*_stencilFunction, *_stencilReference, *_stencilMask, *_stencilFailOp, *_stencilZFailOp, *_stencilZPassOp);
    context->setScissorTest(*_scissorTest, *_scissorBox);
    context->setTriangleCulling(*_triangleCulling);

    if (_pass->isPostProcessing())
        context->drawTriangles(0, 2);
    else
        context->drawTriangles(*_indexBuffer, *_firstIndex, *_numIndices / 3);
}

data::ResolvedBinding*
DrawCall::resolveBinding(const std::string&                                     inputName,
                         const std::unordered_map<std::string, data::Binding>&  bindings)
{
    bool isCollection = false;
    std::string bindingName = inputName;
    bool isArray = inputName[inputName.length() - 1] == ']';
    auto pos = bindingName.find_first_of('[');

    if (!isArray && pos != std::string::npos)
    {
        bindingName = bindingName.substr(0, pos);
        isCollection = true;
    }

    const data::Binding* binding = nullptr;
    std::string bindingPropertyName;

    // Some OpenGL drivers will provide uniform array names without the "[0]" suffix. In order to properly match uniform array
    // bindings, we will check for bindings with 1) the original name first but also 2) the named with the "[0]" suffix appened.
    if (bindings.count(bindingName) != 0 || (!isArray && bindings.count(bindingName += "[0]") != 0))
    {
        binding = &bindings.at(bindingName);
        bindingPropertyName = binding->propertyName;
        // isCollection = isCollection && bindingPropertyName.find_first_of('[') == std::string::npos;
    }
    // else
    // {
    //     for (const auto& inputNameAndBinding : bindings)
    //     {
    //         std::regex r(inputNameAndBinding.first);
    //
    //         if (std::regex_match(inputName, r))
    //         {
    //             bindingPropertyName = std::regex_replace(inputName, r, inputNameAndBinding.second.propertyName);
    //             binding = &inputNameAndBinding.second;
    //             isCollection = false;
    //             break;
    //         }
    //     }

        if (!binding)
            return nullptr;
    // }

    auto& store = getStore(binding->source);
    auto propertyName = data::Store::getActualPropertyName(_variables, bindingPropertyName);

    // FIXME: handle uniforms with struct types

    // FIXME: we assume the uniform is an array of struct or the code to be irrelevantly slow here
    // uniform arrays of non-struct types should be detected and handled as such using a single call
    // to the context providing the direct pointer to the contiguous stored data

    // FIXME: handle per-fields bindings instead of using the raw uniform suffix
    if (isCollection && !isArray)
        propertyName += inputName.substr(pos);

    return new data::ResolvedBinding(*binding, propertyName, store);
}

math::vec3
DrawCall::getEyeSpacePosition()
{
    auto eyePosition = _zSorter->getEyeSpacePosition();

    return eyePosition;
}
