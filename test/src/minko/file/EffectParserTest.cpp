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

#include "EffectParserTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::file;

Effect::Ptr
EffectParserTest::loadEffect(const std::string& filename)
{   
    auto lib = AssetLibrary::create(MinkoTests::canvas()->context());

    lib->loader()->queue(filename);
    lib->loader()->load();

    return lib->effect(filename);
}

TEST_F(EffectParserTest, OneAttributeBinding)
{
    auto fx = loadEffect("effect/OneAttributeBinding.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").propertyName, "geometry[${geometryUuid}].position");
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, TwoAttributeBindings)
{
    auto fx = loadEffect("effect/TwoAttributeBindings.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.size(), 2);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").propertyName, "geometry[${geometryUuid}].position");
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aPosition").source, data::Binding::Source::TARGET);
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aUv").propertyName, "geometry[${geometryUuid}].uv");
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().bindings.at("aUv").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, OneUniformBinding)
{
    auto fx = loadEffect("effect/OneUniformBinding.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseMap").propertyName, "material[${materialUuid}].diffuseMap");
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseMap").source, data::Binding::Source::TARGET);
}

/*** States ***/

/** State default values **/

TEST_F(EffectParserTest, StatesDefaultValues)
{
    auto fx = loadEffect("effect/StatesDefaultValues.effect");
    auto& states = fx->techniques().at("default")[0]->states();

    ASSERT_EQ(states.priority(), States::DEFAULT_PRIORITY);
    ASSERT_EQ(states.zSorted(), States::DEFAULT_ZSORTED);
    ASSERT_EQ(states.blendingSourceFactor(), States::DEFAULT_BLENDING_SOURCE);
    ASSERT_EQ(states.blendingDestinationFactor(), States::DEFAULT_BLENDING_DESTINATION);
    ASSERT_EQ(states.colorMask(), States::DEFAULT_COLOR_MASK);
    ASSERT_EQ(states.depthMask(), States::DEFAULT_DEPTH_MASK);
    ASSERT_EQ(states.depthFunction(), States::DEFAULT_DEPTH_FUNCTION);
    ASSERT_EQ(states.triangleCulling(), States::DEFAULT_TRIANGLE_CULLING);
    ASSERT_EQ(states.stencilFunction(), States::DEFAULT_STENCIL_FUNCTION);
    ASSERT_EQ(states.stencilReference(), States::DEFAULT_STENCIL_REFERENCE);
    ASSERT_EQ(states.stencilMask(), States::DEFAULT_STENCIL_MASK);
    ASSERT_EQ(states.stencilFailOperation(), States::DEFAULT_STENCIL_FAIL_OP);
    ASSERT_EQ(states.stencilZFailOperation(), States::DEFAULT_STENCIL_ZFAIL_OP);
    ASSERT_EQ(states.stencilZPassOperation(), States::DEFAULT_STENCIL_ZPASS_OP);
    ASSERT_EQ(states.scissorTest(), States::DEFAULT_SCISSOR_TEST);
    ASSERT_EQ(states.scissorBox(), States::DEFAULT_SCISSOR_BOX);

    // FIXME : test render target value
    // FIXME : test render states values
}

/** Priority **/

TEST_F(EffectParserTest, PriorityFloatDefaultValue)
{
    auto fx = loadEffect("effect/PriorityFloatDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().priority(), 42.f);
}

TEST_F(EffectParserTest, PriorityFloatArrayValue)
{
    auto fx = loadEffect("effect/PriorityFloatArrayValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().priority(), 2042.f);
}

/** Depth mask **/

TEST_F(EffectParserTest, StatesDepthMask)
{
    auto fx = loadEffect("effect/StatesDepthMask.effect");

    auto value = fx->techniques().at("default")[0]->states().depthMask();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthMask(), false);
}

/** Depth function **/

TEST_F(EffectParserTest, StatesDepthFunctionAlways)
{
    auto fx = loadEffect("effect/StatesDepthFunctionAlways.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::ALWAYS);
}

TEST_F(EffectParserTest, StatesDepthFunctionEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionGreater)
{
    auto fx = loadEffect("effect/StatesDepthFunctionGreater.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::GREATER);
}

TEST_F(EffectParserTest, StatesDepthFunctionGreaterEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionGreaterEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::GREATER_EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionLess)
{
    auto fx = loadEffect("effect/StatesDepthFunctionLess.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::LESS);
}

TEST_F(EffectParserTest, StatesDepthFunctionLessEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionLessEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::LESS_EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionNever)
{
    auto fx = loadEffect("effect/StatesDepthFunctionNever.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::NEVER);
}

TEST_F(EffectParserTest, StatesDepthFunctionNotEqual)
{
    auto fx = loadEffect("effect/StatesDepthFunctionNotEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::NOT_EQUAL);
}

/** Triangle Culling **/

TEST_F(EffectParserTest, StatesTriangleCullingBack)
{
    auto fx = loadEffect("effect/StatesTriangleCullingBack.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::BACK);
}

TEST_F(EffectParserTest, StatesTriangleCullingBoth)
{
    auto fx = loadEffect("effect/StatesTriangleCullingBoth.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::BOTH);
}

TEST_F(EffectParserTest, StatesTriangleCullingFront)
{
    auto fx = loadEffect("effect/StatesTriangleCullingFront.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::FRONT);
}

TEST_F(EffectParserTest, StatesTriangleCullingNone)
{
    auto fx = loadEffect("effect/StatesTriangleCullingNone.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::NONE);
}

/** Blend mode **/

TEST_F(EffectParserTest, StatesBlendModeDefault)
{
    auto fx = loadEffect("effect/StatesBlendModeDefault.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ZERO);
}

TEST_F(EffectParserTest, StatesBlendModeAdditive)
{
    auto fx = loadEffect("effect/StatesBlendModeAdditive.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE);
}

TEST_F(EffectParserTest, StatesBlendModeAlpha)
{
    auto fx = loadEffect("effect/StatesBlendModeAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_ALPHA);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE_MINUS_SRC_ALPHA);
}