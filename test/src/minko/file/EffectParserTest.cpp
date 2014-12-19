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

/*** States bindings ***/

TEST_F(EffectParserTest, StatesBindingPriority)
{
    auto fx = loadEffect("effect/StatesBindingPriority.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("priority").propertyName, "material[${materialUuid}].priority");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("priority").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingZSorted)
{
    auto fx = loadEffect("effect/StatesBindingZSorted.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("zSorted").propertyName, "material[${materialUuid}].zSorted");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("zSorted").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingBlendMode)
{
    auto fx = loadEffect("effect/StatesBindingBlendMode.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendMode").propertyName, "material[${materialUuid}].blendMode");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("blendMode").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingColorMask)
{
    auto fx = loadEffect("effect/StatesBindingColorMask.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("colorMask").propertyName, "material[${materialUuid}].colorMask");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("colorMask").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingDepthMask)
{
    auto fx = loadEffect("effect/StatesBindingDepthMask.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthMask").propertyName, "material[${materialUuid}].depthMask");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthMask").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingDepthFunction)
{
    auto fx = loadEffect("effect/StatesBindingDepthFunction.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthFunction").propertyName, "material[${materialUuid}].depthFunction");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("depthFunction").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingTriangleCulling)
{
    auto fx = loadEffect("effect/StatesBindingTriangleCulling.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("triangleCulling").propertyName, "material[${materialUuid}].triangleCulling");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("triangleCulling").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilFunction)
{
    auto fx = loadEffect("effect/StatesBindingStencilFunction.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFunction").propertyName, "material[${materialUuid}].stencilFunction");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFunction").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilReference)
{
    auto fx = loadEffect("effect/StatesBindingStencilReference.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilReference").propertyName, "material[${materialUuid}].stencilReference");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilReference").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilMask)
{
    auto fx = loadEffect("effect/StatesBindingStencilMask.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilMask").propertyName, "material[${materialUuid}].stencilMask");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilMask").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperation)
{
    auto fx = loadEffect("effect/StatesBindingStencilFailOperation.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFailOperation").propertyName, "material[${materialUuid}].stencilFailOperation");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilFailOperation").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperation)
{
    auto fx = loadEffect("effect/StatesBindingStencilZFailOperation.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZFailOperation").propertyName, "material[${materialUuid}].stencilZFailOperation");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZFailOperation").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperation)
{
    auto fx = loadEffect("effect/StatesBindingStencilZPassOperation.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZPassOperation").propertyName, "material[${materialUuid}].stencilZPassOperation");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("stencilZPassOperation").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingScissorTest)
{
    auto fx = loadEffect("effect/StatesBindingScissorTest.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorTest").propertyName, "material[${materialUuid}].scissorTest");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorTest").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingScissorBox)
{
    auto fx = loadEffect("effect/StatesBindingScissorBox.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorBox").propertyName, "material[${materialUuid}].scissorBox");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("scissorBox").source, data::Binding::Source::TARGET);
}

TEST_F(EffectParserTest, StatesBindingTarget)
{
    auto fx = loadEffect("effect/StatesBindingTarget.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("target").propertyName, "material[${materialUuid}].target");
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at("target").source, data::Binding::Source::TARGET);
}