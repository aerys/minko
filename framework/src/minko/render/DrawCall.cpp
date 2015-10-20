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

#include "minko/data/Store.hpp"
#include "minko/log/Logger.hpp"

// #include <regex>

using namespace minko;
using namespace minko::render;

const unsigned int DrawCall::MAX_NUM_TEXTURES       = 8;
const unsigned int DrawCall::MAX_NUM_VERTEXBUFFERS  = 8;

DrawCall::DrawCall(uint                   batchId,
                   std::shared_ptr<Pass>  pass,
                   const EffectVariables& variables,
                   data::Store&           rootData,
                   data::Store&           rendererData,
                   data::Store&           targetData) :
    _enabled(true),
    _pass(pass),
    _rootData(rootData),
    _rendererData(rendererData),
    _targetData(targetData),
    _variables(variables),
    _indexBuffer(nullptr),
    _firstIndex(nullptr),
    _numIndices(nullptr),
    _priority(&States::DEFAULT_PRIORITY),
    _zSorted(&States::DEFAULT_ZSORTED),
    _blendingSourceFactor(&States::DEFAULT_BLENDING_SOURCE),
    _blendingDestinationFactor(&States::DEFAULT_BLENDING_DESTINATION),
    _colorMask(&States::DEFAULT_COLOR_MASK),
    _depthMask(&States::DEFAULT_DEPTH_MASK),
    _depthFunc(&States::DEFAULT_DEPTH_FUNCTION),
    _triangleCulling(&States::DEFAULT_TRIANGLE_CULLING),
    _stencilFunction(&States::DEFAULT_STENCIL_FUNCTION),
    _stencilReference(&States::DEFAULT_STENCIL_REFERENCE),
    _stencilMask(&States::DEFAULT_STENCIL_MASK),
    _stencilFailOp(&States::DEFAULT_STENCIL_FAIL_OPERATION),
    _stencilZFailOp(&States::DEFAULT_STENCIL_ZFAIL_OPERATION),
    _stencilZPassOp(&States::DEFAULT_STENCIL_ZPASS_OPERATION),
    _scissorTest(&States::DEFAULT_SCISSOR_TEST),
    _scissorBox(&States::DEFAULT_SCISSOR_BOX),
    _target(&States::DEFAULT_TARGET),
    _centerPosition(),
    _modelToWorldMatrix(nullptr),
    _worldToScreenMatrix(nullptr),
    _modelToWorldMatrixPropertyRemovedSlot(nullptr),
    _worldToScreenMatrixPropertyRemovedSlot(nullptr),
    _vertexAttribArray(0)
{
    _batchIDs = { batchId };

    // For Z-sorting
    bindPositionalMembers();
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
    _indexBuffer = nullptr;
    _firstIndex = nullptr;
    _numIndices = nullptr;
    _uniformFloat.clear();
    _uniformInt.clear();
    _uniformBool.clear();
    _samplers.clear();
    _attributes.clear();
    _vertexAttribArray = 0;
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
DrawCall::bindPositionalMembers()
{
    if (_targetData.hasProperty("centerPosition"))
        _centerPosition = _targetData.get<math::vec3>("centerPosition");

    if (_targetData.hasProperty("modelToWorldMatrix"))
        _modelToWorldMatrix = _targetData.getPointer<math::mat4>("modelToWorldMatrix");
    else
    {
        _modelToWorldMatrixPropertyAddedSlot = _targetData.propertyAdded("modelToWorldMatrix").connect(
            [&](data::Store&, std::shared_ptr<data::Provider>, const data::Provider::PropertyName&)
        {
            _modelToWorldMatrix = _targetData.getPointer<math::mat4>("modelToWorldMatrix");
        });
    }

    if (_rendererData.hasProperty("worldToScreenMatrix"))
        _worldToScreenMatrix = _rendererData.getPointer<math::mat4>("worldToScreenMatrix");
    else
    {
        _worldToScreenMatrixPropertyAddedSlot = _rendererData.propertyAdded("worldToScreenMatrix").connect(
            [&](data::Store& store, std::shared_ptr<data::Provider> data, const data::Provider::PropertyName&)
        {
            _worldToScreenMatrix = _rendererData.getPointer<math::mat4>("worldToScreenMatrix");
        });
    }

    // Removed slot
    _modelToWorldMatrixPropertyRemovedSlot = _targetData.propertyRemoved("modelToWorldMatrix").connect(
        [&](data::Store&, std::shared_ptr<data::Provider>, const data::Provider::PropertyName&)
    {
        _modelToWorldMatrix = nullptr;
    });

    _worldToScreenMatrixPropertyRemovedSlot = _rendererData.propertyRemoved("worldToScreenMatrix").connect(
        [&](data::Store& store, std::shared_ptr<data::Provider> data, const data::Provider::PropertyName&)
    {
        _worldToScreenMatrix = nullptr;
    });
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
                LOG_ERROR(
                    "Program \"" + _program->name() + "\": the attribute \"" + input.name
                    + "\" is not bound, has not been set and no default value was provided."
                    );

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
                LOG_ERROR(
                    "Program \"" + _program->name() + "\": the attribute \""
                    + input.name + "\" is bound to the \"" + binding->propertyName
                    + "\" property but it's not defined and no default value was provided."
                );

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
                LOG_ERROR(
                    "Program \"" + _program->name() + "\": the uniform \"" + input.name
                    + "\" is not bound, has not been set and no default value was provided."
                );

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
                LOG_ERROR(
                    "Program \"" + _program->name() + "\": the uniform \""
                    + input.name + "\" is bound to the \"" + binding->propertyName
                    + "\" property but it's not defined and no default value was provided."
                );

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
    bool isArray = input.name[input.name.size() - 1] == ']';

    switch (input.type)
    {
        case ProgramInputs::Type::bool1:
            setUniformValue(_uniformBool, input.location, 1, input.size, (!isArray ? store.getPointer<int>(propertyName) : &store.get<std::vector<int>>(propertyName)[0]));
            break;
        case ProgramInputs::Type::bool2:
            setUniformValue(_uniformBool, input.location, 2, input.size, (!isArray ? math::value_ptr(store.get<math::ivec2>(propertyName)) : math::value_ptr(store.get<std::vector<math::ivec2>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::bool3:
            setUniformValue(_uniformBool, input.location, 3, input.size, (!isArray ? math::value_ptr(store.get<math::ivec3>(propertyName)) : math::value_ptr(store.get<std::vector<math::ivec3>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::bool4:
            setUniformValue(_uniformBool, input.location, 4, input.size, (!isArray ? math::value_ptr(store.get<math::ivec4>(propertyName)) : math::value_ptr(store.get<std::vector<math::ivec4>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::int1:
            setUniformValue(_uniformInt, input.location, 1, input.size, (!isArray ? store.getPointer<int>(propertyName) : &store.get<std::vector<int>>(propertyName)[0]));
            break;
        case ProgramInputs::Type::int2:
            setUniformValue(_uniformInt, input.location, 2, input.size, (!isArray ? math::value_ptr(store.get<math::ivec2>(propertyName)) : math::value_ptr(store.get<std::vector<math::ivec2>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::int3:
            setUniformValue(_uniformInt, input.location, 3, input.size, (!isArray ? math::value_ptr(store.get<math::ivec3>(propertyName)) : math::value_ptr(store.get<std::vector<math::ivec3>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::int4:
            setUniformValue(_uniformInt, input.location, 4, input.size, (!isArray ? math::value_ptr(store.get<math::ivec4>(propertyName)) : math::value_ptr(store.get<std::vector<math::ivec4>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::float1:
            setUniformValue(_uniformFloat, input.location, 1, input.size, (!isArray ? store.getPointer<float>(propertyName) : &store.get<std::vector<float>>(propertyName)[0]));
            break;
        case ProgramInputs::Type::float2:
            setUniformValue(_uniformFloat, input.location, 2, input.size, (!isArray ? math::value_ptr(store.get<math::vec2>(propertyName)) : math::value_ptr(store.get<std::vector<math::vec2>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::float3:
            setUniformValue(_uniformFloat, input.location, 3, input.size, (!isArray ? math::value_ptr(store.get<math::vec3>(propertyName)) : math::value_ptr(store.get<std::vector<math::vec3>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::float4:
            setUniformValue(_uniformFloat, input.location, 4, input.size, (!isArray ? math::value_ptr(store.get<math::vec4>(propertyName)) : math::value_ptr(store.get<std::vector<math::vec4>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::float16:
            setUniformValue(_uniformFloat, input.location, 16, input.size, (!isArray ? math::value_ptr(store.get<math::mat4>(propertyName)) : math::value_ptr(store.get<std::vector<math::mat4>>(propertyName)[0])));
            break;
        case ProgramInputs::Type::sampler2d:
        case ProgramInputs::Type::samplerCube:
        {
            auto samplerIt = std::find_if(
                _samplers.begin(),
                _samplers.end(),
                [&input](const SamplerValue& samplerValue) -> bool { return samplerValue.location == input.location; }
            );

            if (samplerIt == _samplers.end())
            {
                _samplers.push_back({
                    static_cast<uint>(_program->setTextureNames().size() + _samplers.size()),
                    store.getPointer<TextureSampler>(propertyName),
                    input.location
                });
            }
            else
            {
                samplerIt->sampler = store.getPointer<TextureSampler>(propertyName);
            }
        }
        break;
        case ProgramInputs::Type::float9:
        case ProgramInputs::Type::unknown:
            LOG_ERROR("unsupported program input type: " + ProgramInputs::typeToString(input.type));
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
DrawCall::setStateValueFromStore(const std::string&   stateName,
                                 const data::Store&   store)
{
    if (stateName == States::PROPERTY_PRIORITY)
    {
        if (store.hasProperty(stateName))
            _priority = store.getUnsafePointer<float>(stateName);
        else
            _priority = &States::DEFAULT_PRIORITY;
    }
    else if (stateName == States::PROPERTY_ZSORTED)
    {
        if (store.hasProperty(stateName))
            _zSorted = store.getUnsafePointer<bool>(stateName);
        else
            _zSorted = &States::DEFAULT_ZSORTED;
    }
    else if (stateName == States::PROPERTY_BLENDING_SOURCE)
    {
        if (store.hasProperty(stateName))
            _blendingSourceFactor = store.getUnsafePointer<Blending::Source>(stateName);
        else
            _blendingSourceFactor = &States::DEFAULT_BLENDING_SOURCE;
    }
    else if (stateName == States::PROPERTY_BLENDING_DESTINATION)
    {
        if (store.hasProperty(stateName))
            _blendingDestinationFactor = store.getUnsafePointer<Blending::Destination>(stateName);
        else
            _blendingDestinationFactor = &States::DEFAULT_BLENDING_DESTINATION;
    }
    else if (stateName == States::PROPERTY_COLOR_MASK)
    {
        if (store.hasProperty(stateName))
            _colorMask = store.getUnsafePointer<bool>(stateName);
        else
            _colorMask = &States::DEFAULT_COLOR_MASK;
    }
    else if (stateName == States::PROPERTY_DEPTH_MASK)
    {
        if (store.hasProperty(stateName))
            _depthMask = store.getUnsafePointer<bool>(stateName);
        else
            _depthMask = &States::DEFAULT_DEPTH_MASK;
    }
    else if (stateName == States::PROPERTY_DEPTH_FUNCTION)
    {
        if (store.hasProperty(stateName))
            _depthFunc = store.getUnsafePointer<CompareMode>(stateName);
        else
            _depthFunc = &States::DEFAULT_DEPTH_FUNCTION;
    }
    else if (stateName == States::PROPERTY_TRIANGLE_CULLING)
    {
        if (store.hasProperty(stateName))
            _triangleCulling = store.getUnsafePointer<TriangleCulling>(stateName);
        else
            _triangleCulling = &States::DEFAULT_TRIANGLE_CULLING;
    }
    else if (stateName == States::PROPERTY_STENCIL_FUNCTION)
    {
        if (store.hasProperty(stateName))
            _stencilFunction = store.getUnsafePointer<CompareMode>(stateName);
        else
            _stencilFunction = &States::DEFAULT_STENCIL_FUNCTION;
    }
    else if (stateName == States::PROPERTY_STENCIL_REFERENCE)
    {
        if (store.hasProperty(stateName))
            _stencilReference = store.getUnsafePointer<int>(stateName);
        else
            _stencilReference = &States::DEFAULT_STENCIL_REFERENCE;
    }
    else if (stateName == States::PROPERTY_STENCIL_MASK)
    {
        if (store.hasProperty(stateName))
            _stencilMask = store.getUnsafePointer<uint>(stateName);
        else
            _stencilMask = &States::DEFAULT_STENCIL_MASK;
    }
    else if (stateName == States::PROPERTY_STENCIL_FAIL_OPERATION)
    {
        if (store.hasProperty(stateName))
            _stencilFailOp = store.getUnsafePointer<StencilOperation>(stateName);
        else
            _stencilFailOp = &States::DEFAULT_STENCIL_FAIL_OPERATION;
    }
    else if (stateName == States::PROPERTY_STENCIL_ZFAIL_OPERATION)
    {
        if (store.hasProperty(stateName))
            _stencilZFailOp = store.getUnsafePointer<StencilOperation>(stateName);
        else
            _stencilZFailOp = &States::DEFAULT_STENCIL_ZFAIL_OPERATION;
    }
    else if (stateName == States::PROPERTY_STENCIL_ZPASS_OPERATION)
    {
        if (store.hasProperty(stateName))
            _stencilZPassOp = store.getUnsafePointer<StencilOperation>(stateName);
        else
            _stencilZPassOp = &States::DEFAULT_STENCIL_ZPASS_OPERATION;
    }
    else if (stateName == States::PROPERTY_SCISSOR_TEST)
    {
        if (store.hasProperty(stateName))
            _scissorTest = store.getUnsafePointer<bool>(stateName);
        else
            _scissorTest = &States::DEFAULT_SCISSOR_TEST;
    }
    else if (stateName == States::PROPERTY_SCISSOR_BOX)
    {
        if (store.hasProperty(stateName))
            _scissorBox = store.getUnsafePointer<math::ivec4>(stateName);
        else
            _scissorBox = &States::DEFAULT_SCISSOR_BOX;
    }
    else if (stateName == States::PROPERTY_TARGET)
    {
        if (store.hasProperty(stateName))
            _target = store.getUnsafePointer<TextureSampler>(stateName);
        else
            _target = &States::DEFAULT_TARGET;
    }
}

void
DrawCall::bindIndexBuffer()
{
    auto indexBufferProperty = data::Store::getActualPropertyName(_variables, "geometry[${geometryUuid}].indices");

    if (_targetData.hasProperty(indexBufferProperty))
        _indexBuffer = _targetData.getPointer<int>(indexBufferProperty);

    auto surfaceFirstIndexProperty = data::Store::getActualPropertyName(
        _variables,
        "surface[${surfaceUuid}].firstIndex"
    );

    if (!_targetData.hasProperty(surfaceFirstIndexProperty))
    {
        auto geometryFirstIndexProperty = data::Store::getActualPropertyName(
            _variables,
            "geometry[${geometryUuid}].firstIndex"
        );

        if (_targetData.hasProperty(geometryFirstIndexProperty))
            _firstIndex = _targetData.getPointer<uint>(geometryFirstIndexProperty);
    }
    else
    {
        _firstIndex = _targetData.getPointer<uint>(surfaceFirstIndexProperty);
    }

    auto surfaceNumIndicesProperty = data::Store::getActualPropertyName(
        _variables,
        "surface[${surfaceUuid}].numIndices"
    );

    if (!_targetData.hasProperty(surfaceNumIndicesProperty))
    {
        auto geometryNumIndicesProperty = data::Store::getActualPropertyName(
            _variables,
            "geometry[${geometryUuid}].numIndices"
        );

        if (_targetData.hasProperty(geometryNumIndicesProperty))
            _numIndices = _targetData.getPointer<uint>(geometryNumIndicesProperty);
    }
    else
    {
        _numIndices = _targetData.getPointer<uint>(surfaceNumIndicesProperty);
    }
}

data::ResolvedBinding*
DrawCall::bindState(const std::string&        					            stateName,
                    const std::unordered_map<std::string, data::Binding>&     bindings,
                    const data::Store&                                        defaultValues)
{
    auto binding = resolveBinding(
        stateName,
        bindings
    );

    if (binding == nullptr)
    {
        setStateValueFromStore(stateName, defaultValues);
    }
    else
    {
        if (!binding->store.hasProperty(binding->propertyName))
            setStateValueFromStore(stateName, defaultValues);
        else
            setStateValueFromStore(stateName, binding->store);
    }

    return binding;
}

std::array<data::ResolvedBinding*, 17>
DrawCall::bindStates(const std::unordered_map<std::string, data::Binding>&  stateBindings,
                     const data::Store&                                     defaultValues)
{
    std::array<data::ResolvedBinding*, 17> statesResolveBindings = {
        bindState(States::PROPERTY_PRIORITY, stateBindings, defaultValues),
        bindState(States::PROPERTY_ZSORTED, stateBindings, defaultValues),
        bindState(States::PROPERTY_BLENDING_SOURCE, stateBindings, defaultValues),
        bindState(States::PROPERTY_BLENDING_DESTINATION, stateBindings, defaultValues),
        bindState(States::PROPERTY_COLOR_MASK, stateBindings, defaultValues),
        bindState(States::PROPERTY_DEPTH_MASK, stateBindings, defaultValues),
        bindState(States::PROPERTY_DEPTH_FUNCTION, stateBindings, defaultValues),
        bindState(States::PROPERTY_TRIANGLE_CULLING, stateBindings, defaultValues),
        bindState(States::PROPERTY_STENCIL_FUNCTION, stateBindings, defaultValues),
        bindState(States::PROPERTY_STENCIL_REFERENCE, stateBindings, defaultValues),
        bindState(States::PROPERTY_STENCIL_MASK, stateBindings, defaultValues),
        bindState(States::PROPERTY_STENCIL_FAIL_OPERATION, stateBindings, defaultValues),
        bindState(States::PROPERTY_STENCIL_ZFAIL_OPERATION, stateBindings, defaultValues),
        bindState(States::PROPERTY_STENCIL_ZPASS_OPERATION, stateBindings, defaultValues),
        bindState(States::PROPERTY_SCISSOR_TEST, stateBindings, defaultValues),
        bindState(States::PROPERTY_SCISSOR_BOX, stateBindings, defaultValues),
        bindState(States::PROPERTY_TARGET, stateBindings, defaultValues)
    };

    return statesResolveBindings;
}

void
DrawCall::render(AbstractContext::Ptr   context,
                 AbstractTexture::Ptr   renderTarget,
                 const math::ivec4&     viewport,
                 uint                   clearColor)
{
    if (!this->enabled())
        return;

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
        context->setTextureAt(s.position, *s.sampler->id, s.location);
        context->setSamplerStateAt(s.position, *s.wrapMode, *s.textureFilter, *s.mipFilter);
    }

    if (_vertexAttribArray == 0)
    {
        _vertexAttribArray = context->createVertexAttributeArray();

        if (_vertexAttribArray != -1)
        {
            context->setVertexAttributeArray(_vertexAttribArray);
            for (const auto& a : _attributes)
                context->setVertexBufferAt(a.location, *a.resourceId, a.size, *a.stride, a.offset);
        }
    }
    if (_vertexAttribArray != -1)
        context->setVertexAttributeArray(_vertexAttribArray);
    else
        for (const auto& a : _attributes)
            context->setVertexBufferAt(a.location, *a.resourceId, a.size, *a.stride, a.offset);
    
    context->setColorMask(*_colorMask);
    context->setBlendingMode(*_blendingSourceFactor, *_blendingDestinationFactor);
    context->setDepthTest(*_depthMask, *_depthFunc);
    context->setStencilTest(*_stencilFunction, *_stencilReference, *_stencilMask, *_stencilFailOp, *_stencilZFailOp, *_stencilZPassOp);
    context->setScissorTest(*_scissorTest, *_scissorBox);
    context->setTriangleCulling(*_triangleCulling);

    if (!_pass->isForward())
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
    auto modelView = math::mat4();

    if (_modelToWorldMatrix != nullptr)
        modelView = *_modelToWorldMatrix;
    if (_worldToScreenMatrix != nullptr)
        modelView = (*_worldToScreenMatrix) * modelView;

    return math::vec3(modelView * math::vec4(_centerPosition, 1));
}
