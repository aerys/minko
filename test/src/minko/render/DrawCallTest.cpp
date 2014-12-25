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

    DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);
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

    std::map<std::string, data::Binding> bindings = { { "uFoo", { "foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input = { "uFoo", 23, ProgramInputs::Type::float1 };

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

    std::map<std::string, data::Binding> bindings = { { "uFoo", { "foos[${bar}].foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, {{ "bar", "0" }}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input = { "uFoo", 23, ProgramInputs::Type::float1 };

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

    std::map<std::string, data::Binding> bindings = { { "uFoo", { "foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input = { "uFoo", 23, ProgramInputs::Type::int1 };

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

    std::map<std::string, data::Binding> bindings = { { "uFoo", { "foos[${bar}].foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, { { "bar", "0" } }, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input = { "uFoo", 23, ProgramInputs::Type::int1 };

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

    std::map<std::string, data::Binding> bindings = { { "uFoo", { "foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input = { "uFoo", 23, ProgramInputs::Type::bool1 };

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

    std::map<std::string, data::Binding> bindings = { { "uFoo", { "foos[${bar}].foo", data::Binding::Source::ROOT } } };
    DrawCall drawCall(nullptr, { { "bar", "0" } }, rootData, rendererData, targetData);
    ProgramInputs::UniformInput input = { "uFoo", 23, ProgramInputs::Type::bool1 };

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues) != nullptr;

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundBoolUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundIntUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].data, p->getUnsafePointer<int>("foo"));
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundBoolUniforms()[0].size, 1);
}

TEST_F(DrawCallTest, RenderTargetFromDefaultValues)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    auto p = data::Provider::create();

    p->set(States::PROPERTY_TARGET, texture->sampler());
    defaultValues.addProvider(p);

    DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);

    drawCall.bindStates({}, defaultValues);

    ASSERT_NE(drawCall.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(drawCall.target(), texture->sampler());
}

TEST_F(DrawCallTest, RenderTargetBindingFromTargetData)
{
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    data::Store defaultValues;

    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    auto p = data::Provider::create();

    p->set("renderTargetTest", texture->sampler());
    targetData.addProvider(p);

    std::map<std::string, data::Binding> bindings = { { "target", { "renderTargetTest", data::Binding::Source::TARGET } } };
    DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);

    drawCall.bindStates(bindings, defaultValues);

    ASSERT_NE(drawCall.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(drawCall.target(), texture->sampler());
}
