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

#include "minko/data/Container.hpp"

using namespace minko;
using namespace minko::render;

const unsigned int	DrawCall::MAX_NUM_TEXTURES		= 8;
const unsigned int	DrawCall::MAX_NUM_VERTEXBUFFERS	= 8;

const data::Container&
DrawCall::getContainer(data::BindingSource source)
{
    switch (source)
    {
        case data::BindingSource::ROOT:
            return _rootData;
        case data::BindingSource::RENDERER:
            return _rendererData;
        case data::BindingSource::TARGET:
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
DrawCall::bind(Program::Ptr             program,
               const data::BindingMap&  attributeBindings,
               const data::BindingMap&  uniformBindings,
               const data::BindingMap&  stateBindings)
{
    reset();

    _program = program;

    bindIndexBuffer(_variables, _targetData);
    bindStates(stateBindings);

    for (const auto& input : program->inputs().uniforms())
    {
        if (uniformBindings.count(input.name) == 0)
            continue;

        const auto& binding = uniformBindings.at(input.name);
        auto& container = getContainer(binding.source);

        bindUniform(
            program,
            input,
            container,
            data::Container::getActualPropertyName(_variables, binding.propertyName)
        );
    }
     
    for (const auto& input : program->inputs().attributes())
    {
        if (attributeBindings.count(input.name) == 0)
            continue;

        const auto& binding = attributeBindings.at(input.name);
        auto& container = getContainer(binding.source);

        bindAttribute(
            program,
            input,
            container,
            data::Container::getActualPropertyName(_variables, binding.propertyName)
        );
    }
}

void
DrawCall::bindIndexBuffer(const std::unordered_map<std::string, std::string>&   variables,
                          const data::Container&                                targetData)
{
    _indexBuffer = const_cast<int*>(targetData.getPointer<int>(
        data::Container::getActualPropertyName(variables, "geometry[${geometryUuid}].indices")
    ));
    _firstIndex = const_cast<uint*>(targetData.getPointer<uint>(
        data::Container::getActualPropertyName(variables, "geometry[${geometryUuid}].firstIndex")
    ));
    _numIndices = const_cast<uint*>(targetData.getPointer<uint>(
        data::Container::getActualPropertyName(variables, "geometry[${geometryUuid}].numIndices")
    ));
}

void
DrawCall::bindAttribute(Program::Ptr            program,
                        ConstAttrInputRef       input,
                        const data::Container&  container,
                        const std::string&      propertyName)
{
    const auto& attr = container.getPointer<VertexAttribute>(propertyName);

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
                      const data::Container&  container,
                      const std::string&      propertyName)
{
    switch (input.type)
    {
        case ProgramInputs::Type::int1:
            _uniformInt.push_back({ input.location, 1, container.getPointer<int>(propertyName) });
            break;
        case ProgramInputs::Type::int2:
            _uniformInt.push_back({ input.location, 2, math::value_ptr(container.get<math::ivec2>(propertyName)) });
            break;
        case ProgramInputs::Type::int3:
            _uniformInt.push_back({ input.location, 3, math::value_ptr(container.get<math::ivec3>(propertyName)) });
            break;
        case ProgramInputs::Type::int4:
            _uniformInt.push_back({ input.location, 4, math::value_ptr(container.get<math::ivec4>(propertyName)) });
            break;
        case ProgramInputs::Type::float1:
            _uniformFloat.push_back({ input.location, 1, container.getPointer<float>(propertyName) });
            break;
        case ProgramInputs::Type::float2:
            _uniformFloat.push_back({ input.location, 2, math::value_ptr(container.get<math::vec2>(propertyName)) });
            break;
        case ProgramInputs::Type::float3:
            _uniformFloat.push_back({ input.location, 3, math::value_ptr(container.get<math::vec3>(propertyName)) });
            break;
        case ProgramInputs::Type::float4:
            _uniformFloat.push_back({ input.location, 4, math::value_ptr(container.get<math::vec4>(propertyName)) });
            break;
        case ProgramInputs::Type::float16:
            _uniformFloat.push_back({ input.location, 16, math::value_ptr(container.get<math::mat4>(propertyName)) });
            break;
        case ProgramInputs::Type::bool1:
            _uniformBool.push_back({ input.location, 1, container.getPointer<bool>(propertyName) });
            break;
        case ProgramInputs::Type::bool2:
            _uniformBool.push_back({ input.location, 2, math::value_ptr(container.get<math::bvec2>(propertyName)) });
            break;
        case ProgramInputs::Type::bool3:
            _uniformBool.push_back({ input.location, 3, math::value_ptr(container.get<math::bvec3>(propertyName)) });
            break;
        case ProgramInputs::Type::bool4:
            _uniformBool.push_back({ input.location, 4, math::value_ptr(container.get<math::bvec4>(propertyName)) });
            break;
        case ProgramInputs::Type::sampler2d:
            _samplers.push_back({
                program->setTextureNames().size() + _samplers.size(),
                container.getPointer<TextureSampler>(propertyName)->id,
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
    _priority = bindState<float>("priority", stateBindings, &States::DEFAULT_PRIORITY);
    _zsorted = bindState<bool>("zSort", stateBindings, &States::DEFAULT_ZSORTED);
    _blendingSourceFactor = bindState<Blending::Source>(
        "blendingSourceFactor",
        stateBindings,
        &States::DEFAULT_BLENDING_SOURCE
    );
    _blendingDestinationFactor = bindState<Blending::Destination>(
        "blendingDestinationFactor",
        stateBindings,
        &States::DEFAULT_BLENDING_DESTINATION
    );
    _colorMask = bindState<bool>("colorMask", stateBindings, &States::DEFAULT_COLOR_MASK);
    _depthMask = bindState<bool>("depthMask", stateBindings, &States::DEFAULT_DEPTH_MASK);
    _depthFunc = bindState<CompareMode>("depthFunction", stateBindings, &States::DEFAULT_DEPTH_FUNC);
    _triangleCulling = bindState<TriangleCulling>("triangleCulling", stateBindings, &States::DEFAULT_TRIANGLE_CULLING);
    _stencilFunction = bindState<CompareMode>("stencilFunction", stateBindings, &States::DEFAULT_STENCIL_FUNCTION);
    _stencilReference = bindState<int>("stencilReference", stateBindings, &States::DEFAULT_STENCIL_REFERENCE);
    _scissorTest = bindState<bool>("scissorTest", stateBindings, &States::DEFAULT_SCISSOR_TEST);
    _scissorBox = bindState<ScissorBox>("scissorBox", stateBindings, &States::DEFAULT_SCISSOR_BOX);
}

void
DrawCall::render(AbstractContext::Ptr context, AbstractTexture::Ptr renderTarget) const
{
    context->setProgram(_program->id());

    // FIXME: handle RTT
    /*if (_states->target())
        context->setRenderToTexture(_states->target()->id(), true);
    else if (renderTarget)
        context->setRenderToTexture(renderTarget->id(), true);*/

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

    context->setColorMask(*_colorMask);
    context->setBlendMode(*_blendingSourceFactor, *_blendingDestinationFactor);
    context->setDepthTest(*_depthMask, *_depthFunc);
    context->setStencilTest(*_stencilFunction, *_stencilReference, *_stencilMask, *_stencilFailOp, *_stencilZFailOp, *_stencilZPassOp);
    context->setScissorTest(*_scissorTest, *_scissorBox);
    context->setTriangleCulling(*_triangleCulling);

    for (const auto& s : _samplers)
        context->setTextureAt(s.position, *s.resourceId, s.location);

    for (const auto& a : _attributes)
        context->setVertexBufferAt(a.position, *a.resourceId, a.size, *a.stride, a.offset);

    context->drawTriangles(*_indexBuffer, *_numIndices / 3);
}
