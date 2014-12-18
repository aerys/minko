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

using namespace minko;
using namespace minko::render;

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

    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues);

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].data, rootData.getUnsafePointer<float>("foo"));
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].size, 1);
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

    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 0);

    bool uniformIsBound = drawCall.bindUniform(input, bindings, defaultValues);

    ASSERT_TRUE(uniformIsBound);
    ASSERT_EQ(drawCall.boundFloatUniforms().size(), 1);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].data, p->getUnsafePointer<float>("foo"));
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].location, 23);
    ASSERT_EQ(drawCall.boundFloatUniforms()[0].size, 1);
}
