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

using namespace minko;
using namespace minko::render;

const unsigned int	DrawCall::MAX_NUM_TEXTURES		= 8;
const unsigned int	DrawCall::MAX_NUM_VERTEXBUFFERS	= 8;

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
DrawCall::bind(Program::Ptr       program,
               data::BindingMap&  attributeBindings,
               data::BindingMap&  uniformBindings,
               data::BindingMap&  stateBindings)
{
    reset();

    _program = program;

    bindIndexBuffer(_variables, _targetData);
    bindStates(stateBindings);
    bindUniforms(program, uniformBindings);
     
    for (const auto& input : program->inputs().attributes())
    {
        auto& bindings = attributeBindings.bindings;

        if (bindings.count(input.name) == 0)
            continue;

        const auto& binding = bindings.at(input.name);
        auto& store = getStore(binding.source);
        auto propertyName = data::Store::getActualPropertyName(_variables, binding.propertyName);

        if (!store.hasProperty(propertyName))
        {
            if (!attributeBindings.defaultValues.hasProperty(input.name))
                throw std::runtime_error(
                    "The attribute \"" + input.name + "\" is bound to the \"" + propertyName
                    + "\" property but it's not defined and no default value was provided."
                );

            bindAttribute(program, input, attributeBindings.defaultValues, input.name);
        }
        else
            bindAttribute(program, input, store, propertyName);
    }
}

void
DrawCall::bindUniforms(Program::Ptr program, data::BindingMap& uniformBindings)
{
    for (const auto& input : program->inputs().uniforms())
    {
        auto& bindings = uniformBindings.bindings;
        bool isArray = false;
        std::string bindingName = input.name;
        auto pos = bindingName.find_first_of('[');

        if (pos != std::string::npos)
        {
            bindingName = bindingName.substr(0, pos);
            isArray = true;
        }

        if (bindings.count(bindingName) == 0)
            continue;

        const auto& binding = bindings.at(bindingName);
        auto& store = getStore(binding.source);
        auto propertyName = data::Store::getActualPropertyName(_variables, binding.propertyName);

        // FIXME: handle uniforms with struct types

        // FIXME: we assume the uniform is an array of struct or the code to be irrelevantly slow here
        // uniform arrays of non-struct types should be detected and handled as such using a single call
        // to the context providing the direct pointer to the contiguous stored data

        // FIXME: handle per-fields bindings instead of using the raw uniform suffix
        if (isArray)
            propertyName += input.name.substr(pos);

        if (!store.hasProperty(propertyName))
        {
            if (!uniformBindings.defaultValues.hasProperty(input.name))
                throw std::runtime_error(
                    "The uniform \"" + input.name + "\" is bound to the \"" + propertyName
                    + "\" property but it's not defined and no default value was provided."
                );

            bindUniform(program, input, uniformBindings.defaultValues, input.name);

            // FIXME: keep a direct pointer to the uniform data pointer instead of eventually search for it
            _propAddedOrRemovedSlot[&binding] = store.propertyAdded(input.name).connect(std::bind(
                &DrawCall::uniformBindingPropertyAdded,
                this,
                std::ref(binding),
                program,
                std::ref(store),
                std::ref(uniformBindings.defaultValues),
                std::ref(input),
                propertyName
            ));
        }
        else
        {
            bindUniform(program, input, store, propertyName);

            // if there is a default value
            if (uniformBindings.defaultValues.hasProperty(input.name))
            {
                // we listen to the "propertyRemoved" signal in order to make sure the uniform data
                // points to the default value data
                _propAddedOrRemovedSlot[&binding] = store.propertyRemoved(propertyName).connect(std::bind(
                    &DrawCall::uniformBindingPropertyRemoved,
                    this,
                    std::ref(binding),
                    program,
                    std::ref(store),
                    std::ref(uniformBindings.defaultValues),
                    std::ref(input),
                    propertyName
                ));
            }
        }
    }
}

void
DrawCall::uniformBindingPropertyAdded(const data::Binding&                  binding,
                                      Program::Ptr                          program,
                                      data::Store&                      store,
                                      const data::Store&                defaultValues,
                                      const ProgramInputs::UniformInput&    input,
                                      const std::string&                    propertyName)
{
    _propAddedOrRemovedSlot.erase(&binding);
    bindUniform(program, input, store, propertyName);
    _propAddedOrRemovedSlot[&binding] = store.propertyRemoved(propertyName).connect(std::bind(
        &DrawCall::uniformBindingPropertyAdded,
        this,
        std::ref(binding),
        program,
        std::ref(store),
        std::ref(defaultValues),
        std::ref(input),
        propertyName
    ));
}

void
DrawCall::uniformBindingPropertyRemoved(const data::Binding&                  binding,
                                        Program::Ptr                          program,
                                        data::Store&                      store,
                                        const data::Store&                defaultValues,
                                        const ProgramInputs::UniformInput&    input,
                                        const std::string&                    propertyName)
{
    if (!defaultValues.hasProperty(input.name))
        throw std::runtime_error(
        "The uniform \"" + input.name + "\" is bound to the \"" + propertyName
        + "\" property but it's not defined and no default value was provided."
    );

    _propAddedOrRemovedSlot.erase(&binding);
    bindUniform(program, input, defaultValues, input.name);
    _propAddedOrRemovedSlot[&binding] = store.propertyAdded(propertyName).connect(std::bind(
        &DrawCall::uniformBindingPropertyAdded,
        this,
        std::ref(binding),
        program,
        std::ref(store),
        std::ref(defaultValues),
        std::ref(input),
        propertyName
    ));
}

void
DrawCall::bindIndexBuffer(const std::unordered_map<std::string, std::string>&   variables,
                          const data::Store&                                targetData)
{
    _indexBuffer = const_cast<int*>(targetData.getPointer<int>(
        data::Store::getActualPropertyName(variables, "geometry[${geometryUuid}].indices")
    ));
    _firstIndex = const_cast<uint*>(targetData.getPointer<uint>(
        data::Store::getActualPropertyName(variables, "geometry[${geometryUuid}].firstIndex")
    ));
    _numIndices = const_cast<uint*>(targetData.getPointer<uint>(
        data::Store::getActualPropertyName(variables, "geometry[${geometryUuid}].numIndices")
    ));
}

void
DrawCall::bindAttribute(Program::Ptr            program,
                        ConstAttrInputRef       input,
                        const data::Store&  store,
                        const std::string&      propertyName)
{
    const auto& attr = store.getPointer<VertexAttribute>(propertyName);

    _attributes.push_back({
        program->setAttributeNames().size() + _attributes.size(),
        input.location,
        attr->resourceId,
        attr->size,
        attr->vertexSize,
        attr->offset
    });
}

void
DrawCall::bindUniform(Program::Ptr            program,
                      ConstUniformInputRef    input,
                      const data::Store&  store,
                      const std::string&      propertyName)
{
    switch (input.type)
    {
        case ProgramInputs::Type::int1:
            setUniformValue(_uniformInt, input.location, 1, store.getPointer<int>(propertyName));
            break;
        case ProgramInputs::Type::int2:
            setUniformValue(_uniformInt, input.location, 2, math::value_ptr(store.get<math::ivec2>(propertyName)));
            break;
        case ProgramInputs::Type::int3:
            setUniformValue(_uniformInt, input.location, 3, math::value_ptr(store.get<math::ivec3>(propertyName)));
            break;
        case ProgramInputs::Type::int4:
            setUniformValue(_uniformInt, input.location, 4, math::value_ptr(store.get<math::ivec4>(propertyName)));
            break;
        case ProgramInputs::Type::float1:
            setUniformValue(_uniformFloat, input.location, 1, store.getPointer<float>(propertyName));
            break;
        case ProgramInputs::Type::float2:
            setUniformValue(_uniformFloat, input.location, 2, math::value_ptr(store.get<math::vec2>(propertyName)));
            break;
        case ProgramInputs::Type::float3:
            setUniformValue(_uniformFloat, input.location, 3, math::value_ptr(store.get<math::vec3>(propertyName)));
            break;
        case ProgramInputs::Type::float4:
            setUniformValue(_uniformFloat, input.location, 4, math::value_ptr(store.get<math::vec4>(propertyName)));
            break;
        case ProgramInputs::Type::float16:
            setUniformValue(_uniformFloat, input.location, 16, math::value_ptr(store.get<math::mat4>(propertyName)));
            break;
        case ProgramInputs::Type::bool1:
            setUniformValue(_uniformBool, input.location, 1, store.getPointer<bool>(propertyName));
            break;
        case ProgramInputs::Type::bool2:
            setUniformValue(_uniformBool, input.location, 2, math::value_ptr(store.get<math::bvec2>(propertyName)));
            break;
        case ProgramInputs::Type::bool3:
            setUniformValue(_uniformBool, input.location, 3, math::value_ptr(store.get<math::bvec3>(propertyName)));
            break;
        case ProgramInputs::Type::bool4:
            setUniformValue(_uniformBool, input.location, 4, math::value_ptr(store.get<math::bvec4>(propertyName)));
            break;
        case ProgramInputs::Type::sampler2d:
            _samplers.push_back({
                program->setTextureNames().size() + _samplers.size(),
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
DrawCall::bindStates(const data::BindingMap& stateBindings)
{
    const States& s = _pass->states();



    /*_priority = bindState<float>("priority", stateBindings, &def->_priority);
    _zsorted = bindState<bool>("zSort", stateBindings, &def->_zsorted);
    _blendingSourceFactor = bindState<Blending::Source>("blendingSourceFactor", stateBindings, &def->_blendingSourceFactor);
    _blendingDestinationFactor = bindState<Blending::Destination>("blendingDestinationFactor", stateBindings, &def->_blendingDestinationFactor);
    _colorMask = bindState<bool>("colorMask", stateBindings, &def->_colorMask);
    _depthMask = bindState<bool>("depthMask", stateBindings, &def->_depthMask);
    _depthFunc = bindState<CompareMode>("depthFunction", stateBindings, &def->_depthFunc);
    _triangleCulling = bindState<TriangleCulling>("triangleCulling", stateBindings, &def->_triangleCulling);
    _stencilFunction = bindState<CompareMode>("stencilFunction", stateBindings, &def->_stencilFunction);
    _stencilReference = bindState<int>("stencilReference", stateBindings, &def->_stencilReference);
    _stencilMask = bindState<uint>("stencilMask", stateBindings, &def->_stencilMask);
    _stencilFailOp = bindState<StencilOperation>("stencilFailOperation", stateBindings, &def->_stencilFailOp);
    _stencilZFailOp = bindState<StencilOperation>("stencilZFailOperation", stateBindings, &def->_stencilZFailOp);
    _stencilZPassOp = bindState<StencilOperation>("stencilZPassOperation", stateBindings, &def->_stencilZPassOp);
    _scissorTest = bindState<bool>("scissorTest", stateBindings, &def->_scissorTest);
    _scissorBox = bindState<ScissorBox>("scissorBox", stateBindings, &def->_scissorBox);*/

    // FIXME: bind the render target
}

void
DrawCall::render(AbstractContext::Ptr    		context,
                 AbstractTexture::Ptr           renderTarget,
                 const render::ScissorBox&      viewport) const
{
    context->setProgram(_program->id());

    // FIXME: handle RTT
    /*if (_states->target())
        context->setRenderToTexture(_states->target()->id(), true);
    else if (renderTarget)
        context->setRenderToTexture(renderTarget->id(), true);*/

    // FIXME: set viewport according to in arg if not render target is bound
    //   if (viewport.width >= 0 && viewport.height >= 0)
    //   context->configureViewport(viewport.x, viewport.y, viewport.width, viewport.height);


    for (const auto& u : _uniformFloat)
    {
        if (u.size == 1)
            context->setUniformFloat(u.location, 1, u.data);
        else if (u.size == 2)
            context->setUniformFloat2(u.location, 1, u.data);
        else if (u.size == 3)
            context->setUniformFloat3(u.location, 1, u.data);
        else if (u.size == 4)
            context->setUniformFloat4(u.location, 1, u.data);
        else if (u.size == 16)
            context->setUniformMatrix4x4(u.location, 1, u.data);
    }

    for (const auto& u : _uniformInt)
    {
        if (u.size == 1)
            context->setUniformInt(u.location, 1, u.data);
        else if (u.size == 2)
            context->setUniformInt2(u.location, 1, u.data);
        else if (u.size == 3)
            context->setUniformInt3(u.location, 1, u.data);
        else if (u.size == 4)
            context->setUniformInt4(u.location, 1, u.data);
    }

    // FIXME: handle bool uniforms
    /*
    for (const auto& u : _uniformBool)
    {
        if (u.size == 1)
            context->setUniformBool(u.location, 1, u.data);
        else if (u.size == 2)
            context->setUniformBool2(u.location, 1, u.data);
        else if (u.size == 3)
            context->setUniformBool3(u.location, 1, u.data);
        else if (u.size == 4)
            context->setUniformBool4(u.location, 1, u.data);
    }
    */

    /*context->setColorMask(*_colorMask);
    context->setBlendMode(*_blendingSourceFactor, *_blendingDestinationFactor);
    context->setDepthTest(*_depthMask, *_depthFunc);
    context->setStencilTest(*_stencilFunction, *_stencilReference, *_stencilMask, *_stencilFailOp, *_stencilZFailOp, *_stencilZPassOp);
    context->setScissorTest(*_scissorTest, *_scissorBox);
    context->setTriangleCulling(*_triangleCulling);*/

    /*context->setColorMask(true);
    context->setBlendMode(Blending::Mode::DEFAULT);
    context->setDepthTest(true, CompareMode::LESS);
    context->setScissorTest(false, math::vec4(0.f));
    context->setTriangleCulling(TriangleCulling::NONE);*/

    for (const auto& s : _samplers)
        context->setTextureAt(s.position, *s.resourceId, s.location);

    for (const auto& a : _attributes)
        context->setVertexBufferAt(a.location, *a.resourceId, a.size, *a.stride, a.offset);

    context->drawTriangles(*_indexBuffer, *_numIndices / 3);
}
