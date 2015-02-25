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

#include "DrawCallTest.hpp"

#include "minko/data/Provider.hpp"
#include "minko/data/ResolvedBinding.hpp"
#include "minko/render/States.hpp"

using namespace minko;
using namespace minko::render;

std::string
DrawCallTest::randomString(uint len)
{
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string s;

    for (uint i = 0; i < len; ++i)
        s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return s;
}

TEST_F(DrawCallTest, Constructor)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);
}

TEST_F(DrawCallTest, OneFloatUniformBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    p->set<float>("foo", 42.f);
    rootData.addProvider(p);

    std::unordered_map<std::string, data::Binding> bindings = { { "uFoo", { "foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input("uFoo", 23, 1, ProgramInputs::Type::float1);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].data, rootData.getUnsafePointer<float>("foo"));
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, MultipleFloatUniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<float, float>(
        ProgramInputs::Type::float1,
        1,
        []() { return (float)rand(); },
        [](DrawCall& d) { return d.boundFloatUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleFloat2UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::vec2, float>(
        ProgramInputs::Type::float2,
        2,
        []() { return math::diskRand(100.f); },
        [](DrawCall& d) { return d.boundFloatUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleFloat3UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::vec3, float>(
        ProgramInputs::Type::float3,
        3,
        []() { return math::sphericalRand(-100.f); },
        [](DrawCall& d) { return d.boundFloatUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleFloat4UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::vec4, float>(
        ProgramInputs::Type::float4,
        4,
        []()
        {
            return math::vec4(
                math::linearRand(-100.f, 100.f),
                math::linearRand(-100.f, 100.f),
                math::linearRand(-100.f, 100.f),
                math::linearRand(-100.f, 100.f)
            );
        },
            [](DrawCall& d) { return d.boundFloatUniforms(); }
    );
}

TEST_F(DrawCallTest, OneFloatUniformWithVariableBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    p->set<float>("foo", 42.f);
    rootData.addProvider(p, "foos");

    std::unordered_map<std::string, data::Binding> bindings = { { "uFoo", { "foos[${bar}].foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, {{ "bar", "0" }}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input("uFoo", 23, 1, ProgramInputs::Type::float1);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].data, p->getUnsafePointer<float>("foo"));
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, OneIntUniformBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    p->set<int>("foo", 42);
    rootData.addProvider(p);

    std::unordered_map<std::string, data::Binding> bindings = { { "uFoo", { "foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input("uFoo", 23, 1, ProgramInputs::Type::int1);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundIntUniforms()[0].data, rootData.getUnsafePointer<int>("foo"));
    ASSERT_EQ(drawCall.boundIntUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundIntUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, MultipleIntUniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<int, int>(
        ProgramInputs::Type::int1,
        1,
        []() { return rand(); },
        [](DrawCall& d) { return d.boundIntUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleInt2UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::ivec2, int>(
        ProgramInputs::Type::int2,
        2,
        []() { return math::ivec2(rand(), rand()); },
        [](DrawCall& d) { return d.boundIntUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleInt3UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::ivec3, int>(
        ProgramInputs::Type::int3,
        3,
        []() { return math::ivec3(rand(), rand(), rand()); },
        [](DrawCall& d) { return d.boundIntUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleInt4UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::ivec4, int>(
        ProgramInputs::Type::int4,
        4,
        []() { return math::ivec4(rand(), rand(), rand(), rand()); },
        [](DrawCall& d) { return d.boundIntUniforms(); }
    );
}

TEST_F(DrawCallTest, OneIntUniformWithVariableBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    p->set<int>("foo", 42);
    rootData.addProvider(p, "foos");

    std::unordered_map<std::string, data::Binding> bindings = { { "uFoo", { "foos[${bar}].foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, { { "bar", "0" } }, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input("uFoo", 23, 1, ProgramInputs::Type::int1);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundIntUniforms()[0].data, p->getUnsafePointer<int>("foo"));
    ASSERT_EQ(drawCall.boundIntUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundIntUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, OneBoolUniformBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    p->set<int>("foo", 1);
    rootData.addProvider(p);

    std::unordered_map<std::string, data::Binding> bindings = { { "uFoo", { "foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input("uFoo", 23, 1, ProgramInputs::Type::bool1);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].data, rootData.getUnsafePointer<int>("foo"));
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, MultipleBoolUniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<int, int>(
        ProgramInputs::Type::bool1,
        1,
        []() { return rand() % 2; },
        [](DrawCall& d) { return d.boundBoolUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleBool2UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::ivec2, int>(
        ProgramInputs::Type::bool2,
        2,
        []() { return math::ivec2(rand() % 2, rand() % 2); },
        [](DrawCall& d) { return d.boundBoolUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleBool3UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::ivec3, int>(
        ProgramInputs::Type::bool3,
        3,
        []() { return math::ivec3(rand() % 2, rand() % 2, rand() % 2); },
        [](DrawCall& d) { return d.boundBoolUniforms(); }
    );
}

TEST_F(DrawCallTest, MultipleBool4UniformBindingsFromRootData)
{
    testMultipleUniformsFromRootData<math::ivec4, int>(
        ProgramInputs::Type::bool4,
        4,
        []() { return math::ivec4(rand() % 2, rand() % 2, rand() % 2, rand() % 2); },
        [](DrawCall& d) { return d.boundBoolUniforms(); }
    );
}

TEST_F(DrawCallTest, OneBoolUniformWithVariableBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    p->set<int>("foo", 1);
    rootData.addProvider(p, "foos");

    std::unordered_map<std::string, data::Binding> bindings = { { "uFoo", { "foos[${bar}].foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, { { "bar", "0" } }, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input("uFoo", 23, 1, ProgramInputs::Type::bool1);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].data, p->getUnsafePointer<int>("foo"));
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, RenderTargetDefaultValue)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    States states;

    defaultValues.addProvider(states.data());

    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);

    std::unordered_map<std::string, data::Binding> bindings;
    drawCall.bindStates(bindings, defaultValues);

    ASSERT_EQ(drawCall.target(), States::DEFAULT_TARGET);
}

TEST_F(DrawCallTest, RenderTargetFromDefaultValues)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    States states;

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);

    texture->upload();
    states.target(texture->sampler());
    defaultValues.addProvider(states.data());

    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);

    std::unordered_map<std::string, data::Binding> bindings;
    drawCall.bindStates(bindings, defaultValues);

    ASSERT_NE(drawCall.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(drawCall.target(), texture->sampler());
    ASSERT_GT(*drawCall.target().id, 0);
}

TEST_F(DrawCallTest, RenderTargetBindingFromTargetData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    auto p = data::Provider::create();

    texture->upload();
    p->set("renderTargetTest", texture->sampler());
    targetData.addProvider(p);

    std::unordered_map<std::string, data::Binding> bindings = { { "target", { "renderTargetTest", data::Binding::Source::TARGET } } };
    DrawCall drawCall(nullptr, std::unordered_map<std::string, std::string>{}, rootData, rendererData, targetData);

    drawCall.bindStates(bindings, defaultValues);

    ASSERT_NE(drawCall.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(drawCall.target(), texture->sampler());
    ASSERT_GT(*drawCall.target().id, 0);
}

/** Sampler states bindings **/

// Sampler states without bindings and without default values (implicit default values)

TEST_F(DrawCallTest, SamplerStatesImplicitDefaultValues)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } },
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    auto location = 23;
    auto size = 0;

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

// Sampler states with explicit default value

TEST_F(DrawCallTest, SamplerStatesWrapModeWithDefaultValueRepeat)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_WRAP_MODE;
    auto sampleStateUniformDefaultValue = WrapMode::REPEAT;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, defaultValues.get<WrapMode>(sampleStateUniformName));
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallTest, SamplerStatesWrapModeWithDefaultValueClamp)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_WRAP_MODE;
    auto sampleStateUniformDefaultValue = WrapMode::CLAMP;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, defaultValues.get<WrapMode>(sampleStateUniformName));
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallTest, SamplerStatesTextureFilterWithDefaultValueLinear)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;
    auto sampleStateUniformDefaultValue = TextureFilter::LINEAR;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, defaultValues.get<TextureFilter>(sampleStateUniformName));
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallTest, SamplerStatesTextureFilterWithDefaultValueNearest)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;
    auto sampleStateUniformDefaultValue = TextureFilter::NEAREST;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, defaultValues.get<TextureFilter>(sampleStateUniformName));
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallTest, SamplerStatesMipFilterWithDefaultValueLinear)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto sampleStateUniformDefaultValue = MipFilter::LINEAR;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, defaultValues.get<MipFilter>(sampleStateUniformName));
}

TEST_F(DrawCallTest, SamplerStatesMipFilterWithDefaultValueLinearNearest)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto sampleStateUniformDefaultValue = MipFilter::NEAREST;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, defaultValues.get<MipFilter>(sampleStateUniformName));
}

TEST_F(DrawCallTest, SamplerStatesMipFilterWithDefaultValueNone)
{
    auto samplerStatesProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto sampleStateUniformDefaultValue = MipFilter::NONE;

    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();
    auto defaultValueProvider = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStatesProperty
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;

    defaultValues.addProvider(defaultValueProvider);
    defaultValueProvider->set(sampleStateUniformName, sampleStateUniformDefaultValue);

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, defaultValues.get<MipFilter>(sampleStateUniformName));
}

// Sampler states with variable binding

TEST_F(DrawCallTest, SamplerStatesWrapModeWithVariableBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto p = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto wrapModeUniformName = SamplerStates::uniformNameToSamplerStateName(
            samplerUniformName,
            SamplerStates::PROPERTY_WRAP_MODE
    );
    auto wrapModeBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        SamplerStates::PROPERTY_WRAP_MODE
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    p->set<render::WrapMode>(wrapModeBindingName, WrapMode::REPEAT);
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;
    auto wrapModeUniformValue = "material[${id}]." + wrapModeBindingName;

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } },
        { wrapModeUniformName, { wrapModeUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    auto location = 23;
    auto size = 0;

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_NE(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, p->get<WrapMode>(wrapModeBindingName));
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallTest, SamplerStatesTextureFilterWithVariableBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto textureFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
            samplerUniformName,
            SamplerStates::PROPERTY_TEXTURE_FILTER
    );
    auto textureFilterBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        SamplerStates::PROPERTY_TEXTURE_FILTER
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    p->set<render::TextureFilter>(textureFilterBindingName, TextureFilter::LINEAR);
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;
    auto textureFilterUniformValue = "material[${id}]." + textureFilterBindingName;

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } },
        { textureFilterUniformName, { textureFilterUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_NE(resolvedBindings.at(1), nullptr);
    ASSERT_EQ(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, p->get<TextureFilter>(textureFilterBindingName));
    ASSERT_EQ(*samplers.at(0).mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallTest, SamplerStatesMipFilterWithVariableBindingFromRootData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;
    auto location = 23;
    auto size = 0;

    auto p = data::Provider::create();

    auto samplerUniformName = "uDiffuseMap";
    std::string samplerBindingName = "diffuseMap";

    auto mipFilterUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        SamplerStates::PROPERTY_MIP_FILTER
    );
    auto mipFilterBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        SamplerStates::PROPERTY_MIP_FILTER
    );

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    p->set<render::TextureSampler>(samplerBindingName, texture->sampler());
    p->set<render::MipFilter>(mipFilterBindingName, MipFilter::LINEAR);
    rootData.addProvider(p, "material");

    auto samplerUniformValue = "material[${id}]." + samplerBindingName;
    auto mipFilterUniformValue = "material[${id}]." + mipFilterBindingName;

    std::unordered_map<std::string, data::Binding> bindings = {
        { samplerUniformName, { samplerUniformValue, data::Binding::Source::ROOT } },
        { mipFilterUniformName, { mipFilterUniformValue, data::Binding::Source::ROOT } }
    };

    DrawCall drawCall(nullptr, { { "id", "0" } }, rootData, rendererData, targetData);

    ProgramInputs::UniformInput input(samplerUniformName, location, size, ProgramInputs::Type::sampler2d);

    auto vertexShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::VERTEX_SHADER, "");
    auto fragmentShader = Shader::create(MinkoTests::canvas()->context(), Shader::Type::FRAGMENT_SHADER, "");
    vertexShader->upload();
    fragmentShader->upload();

    auto program = Program::create("program", MinkoTests::canvas()->context(), vertexShader, fragmentShader);
    program->upload();

    drawCall.bind(program);

    auto resolveBinding = drawCall.bindUniform(input, bindings, defaultValues);
    auto resolvedBindings = drawCall.bindSamplerStates(input, bindings, defaultValues);

    auto samplers = drawCall.samplers();

    ASSERT_EQ(resolvedBindings.size(), 3);
    ASSERT_EQ(resolvedBindings.at(0), nullptr);
    ASSERT_EQ(resolvedBindings.at(1), nullptr);
    ASSERT_NE(resolvedBindings.at(2), nullptr);

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_EQ(samplers.at(0).location, location);
    ASSERT_EQ(*samplers.at(0).wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*samplers.at(0).textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*samplers.at(0).mipFilter, p->get<MipFilter>(mipFilterBindingName));
}
