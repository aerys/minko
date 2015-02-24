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

/******************/
/*** Attributes ***/
/******************/

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

/****************/
/*** Uniforms ***/
/****************/

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

TEST_F(EffectParserTest, BoolDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/BoolDefaultValue.effect");

    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<int>("testBool1Uniform"), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::ivec2>("testBool2Uniform"), math::ivec2(1, 0));
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::ivec3>("testBool3Uniform"), math::ivec3(1, 0, 1));
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::ivec4>("testBool4Uniform"), math::ivec4(1, 0, 1, 0));
}

TEST_F(EffectParserTest, Float4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/Float4DefaultValue.effect");

    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<math::vec4>("testFloat4Uniform"), math::vec4(1.f));
}

TEST_F(EffectParserTest, OneUniformBindingAndDefault)
{
    auto fx = MinkoTests::loadEffect("effect/OneUniformBindingAndDefault.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseColor").propertyName, "diffuseColor");
    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().bindings.at("uDiffuseColor").source, data::Binding::Source::TARGET);
    ASSERT_TRUE(fx->techniques().at("default")[0]->uniformBindings().defaultValues.hasProperty("uDiffuseColor"));
}

/**************/
/*** Macros ***/
/**************/

TEST_F(EffectParserTest, MacroIntDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroIntDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<int>("TEST_INT_MACRO"), 42);
}

TEST_F(EffectParserTest, MacroInt2DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroInt2DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT2_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec2>("TEST_INT2_MACRO"), math::ivec2(42, 23));
}

TEST_F(EffectParserTest, MacroInt3DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroInt3DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT3_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec3>("TEST_INT3_MACRO"), math::ivec3(42, 23, 13));
}

TEST_F(EffectParserTest, MacroInt4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroInt4DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_INT4_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec4>("TEST_INT4_MACRO"), math::ivec4(42, 23, 13, 7));
}

TEST_F(EffectParserTest, MacroFloatDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloatDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<float>("TEST_FLOAT_MACRO"), 42.24f);
}

TEST_F(EffectParserTest, MacroFloat2DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloat2DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT2_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::vec2>("TEST_FLOAT2_MACRO"), math::vec2(42.24f, 23.32f));
}

TEST_F(EffectParserTest, MacroFloat3DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloat3DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT3_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::vec3>("TEST_FLOAT3_MACRO"), math::vec3(42.24f, 23.32f, 13.31f));
}

TEST_F(EffectParserTest, MacroFloat4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroFloat4DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_FLOAT4_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::vec4>("TEST_FLOAT4_MACRO"), math::vec4(42.24f, 23.32f, 13.31f, 7.7f));
}

TEST_F(EffectParserTest, MacroBoolDefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBoolDefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<int>("TEST_BOOL_MACRO"), 1);
}

TEST_F(EffectParserTest, MacroBool2DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBool2DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL2_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec2>("TEST_BOOL2_MACRO"), math::ivec2(1, 0));
}

TEST_F(EffectParserTest, MacroBool3DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBool3DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL3_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec3>("TEST_BOOL3_MACRO"), math::ivec3(1, 0, 1));
}

TEST_F(EffectParserTest, MacroBool4DefaultValue)
{
    auto fx = MinkoTests::loadEffect("effect/MacroBool4DefaultValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);
    ASSERT_TRUE(fx->techniques().at("default")[0]->macroBindings().defaultValues.hasProperty("TEST_BOOL4_MACRO"));
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.get<math::ivec4>("TEST_BOOL4_MACRO"), math::ivec4(1, 0, 1, 0));
}

/**************/
/*** Passes ***/
/**************/

TEST_F(EffectParserTest, MultiplePassesHaveDifferentStateData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().bindings,
        &fx->techniques().at("default")[1]->stateBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues,
        &fx->techniques().at("default")[1]->stateBindings().defaultValues
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues.providers().front(),
        &fx->techniques().at("default")[1]->stateBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().bindings,
        &fx->techniques().at("default")[2]->stateBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues,
        &fx->techniques().at("default")[2]->stateBindings().defaultValues
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->stateBindings().defaultValues.providers().front(),
        &fx->techniques().at("default")[2]->stateBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->stateBindings().bindings,
        &fx->techniques().at("default")[2]->stateBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->stateBindings().defaultValues,
        &fx->techniques().at("default")[2]->stateBindings().defaultValues
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->stateBindings().defaultValues.providers().front(),
        &fx->techniques().at("default")[2]->stateBindings().defaultValues.providers().front()
    );
    ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->stateBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->stateBindings().defaultValues.providers().size(), 1);
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentUniformData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().bindings,
        &fx->techniques().at("default")[1]->uniformBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().defaultValues,
        &fx->techniques().at("default")[1]->uniformBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[1]->uniformBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().bindings,
        &fx->techniques().at("default")[2]->uniformBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->uniformBindings().defaultValues,
        &fx->techniques().at("default")[2]->uniformBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->uniformBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->uniformBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->uniformBindings().bindings,
        &fx->techniques().at("default")[2]->uniformBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->uniformBindings().defaultValues,
        &fx->techniques().at("default")[2]->uniformBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[1]->uniformBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->uniformBindings().defaultValues.providers().front()
    );

    ASSERT_EQ(fx->techniques().at("default")[0]->uniformBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->uniformBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->uniformBindings().defaultValues.providers().size(), 1);
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentMacroData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().bindings,
        &fx->techniques().at("default")[1]->macroBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().defaultValues,
        &fx->techniques().at("default")[1]->macroBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->macroBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[1]->macroBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().bindings,
        &fx->techniques().at("default")[2]->macroBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->macroBindings().defaultValues,
        &fx->techniques().at("default")[2]->macroBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->macroBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->macroBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->macroBindings().bindings,
        &fx->techniques().at("default")[2]->macroBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->macroBindings().defaultValues,
        &fx->techniques().at("default")[2]->macroBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[1]->macroBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->macroBindings().defaultValues.providers().front()
    );
    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->macroBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->macroBindings().defaultValues.providers().size(), 1);
}

TEST_F(EffectParserTest, MultiplePassesHaveDifferentAttributeData)
{
    auto fx = MinkoTests::loadEffect("effect/MultiplePasses.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 3);
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().bindings,
        &fx->techniques().at("default")[1]->attributeBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().defaultValues,
        &fx->techniques().at("default")[1]->attributeBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->attributeBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[1]->attributeBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().bindings,
        &fx->techniques().at("default")[2]->attributeBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[0]->attributeBindings().defaultValues,
        &fx->techniques().at("default")[2]->attributeBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[0]->attributeBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->attributeBindings().defaultValues.providers().front()
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->attributeBindings().bindings,
        &fx->techniques().at("default")[2]->attributeBindings().bindings
    );
    ASSERT_NE(
        &fx->techniques().at("default")[1]->attributeBindings().defaultValues,
        &fx->techniques().at("default")[2]->attributeBindings().defaultValues
    );
    ASSERT_NE(
        fx->techniques().at("default")[1]->attributeBindings().defaultValues.providers().front(),
        fx->techniques().at("default")[2]->attributeBindings().defaultValues.providers().front()
    );
    ASSERT_EQ(fx->techniques().at("default")[0]->attributeBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[1]->attributeBindings().defaultValues.providers().size(), 1);
    ASSERT_EQ(fx->techniques().at("default")[2]->attributeBindings().defaultValues.providers().size(), 1);

}

/**************/
/*** States ***/
/**************/

/** State default values **/

TEST_F(EffectParserTest, StatesDefaultValues)
{
    auto fx = loadEffect("effect/state/default-value/StatesDefaultValues.effect");
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
    ASSERT_EQ(states.stencilFailOperation(), States::DEFAULT_STENCIL_FAIL_OPERATION);
    ASSERT_EQ(states.stencilZFailOperation(), States::DEFAULT_STENCIL_ZFAIL_OPERATION);
    ASSERT_EQ(states.stencilZPassOperation(), States::DEFAULT_STENCIL_ZPASS_OPERATION);
    ASSERT_EQ(states.scissorTest(), States::DEFAULT_SCISSOR_TEST);
    ASSERT_EQ(states.scissorBox(), States::DEFAULT_SCISSOR_BOX);
    ASSERT_EQ(states.target(), States::DEFAULT_TARGET);
}

/* Priority */

TEST_F(EffectParserTest, StatesPriorityFloatValue)
{
    auto fx = loadEffect("effect/state/default-value/priority/StatesPriorityFloatValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().priority(), 42.f);
}

TEST_F(EffectParserTest, StatesPriorityArrayValue)
{
    auto fx = loadEffect("effect/state/default-value/priority/StatesPriorityArrayValue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().priority(), 2042.f);
}

/* ZSorted */

TEST_F(EffectParserTest, StatesZSortedTrue)
{
    auto fx = loadEffect("effect/state/default-value/zsorted/StatesZSortedTrue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().zSorted(), true);
}

TEST_F(EffectParserTest, StatesZSortedFalse)
{
    auto fx = loadEffect("effect/state/default-value/zsorted/StatesZSortedFalse.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().zSorted(), false);
}

/* Blending mode */

TEST_F(EffectParserTest, StatesBlendingModeDefault)
{
    auto fx = loadEffect("effect/state/default-value/blending-mode/StatesBlendingModeDefault.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ZERO);
}

TEST_F(EffectParserTest, StatesBlendingModeAdditive)
{
    auto fx = loadEffect("effect/state/default-value/blending-mode/StatesBlendingModeAdditive.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_ALPHA);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE);
}

TEST_F(EffectParserTest, StatesBlendingModeAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-mode/StatesBlendingModeAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_ALPHA);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE_MINUS_SRC_ALPHA);
}

TEST_F(EffectParserTest, StatesBlendingModeArray)
{
    auto fx = loadEffect("effect/state/default-value/blending-mode/StatesBlendingModeArray.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_COLOR);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::SRC_ALPHA_SATURATE);
}

/* Blending Source */

TEST_F(EffectParserTest, StatesBlendingSourceZero)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceZero.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ZERO);
}

TEST_F(EffectParserTest, StatesBlendingSourceOne)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceOne.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE);
}

TEST_F(EffectParserTest, StatesBlendingSourceSrcColor)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceSrcColor.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_COLOR);
}

TEST_F(EffectParserTest, StatesBlendingSourceOneMinusSrcColor)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceOneMinusSrcColor.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE_MINUS_SRC_COLOR);
}

TEST_F(EffectParserTest, StatesBlendingSourceSrcAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceSrcAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::SRC_ALPHA);
}

TEST_F(EffectParserTest, StatesBlendingSourceOneMinusSrcAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceOneMinusSrcAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE_MINUS_SRC_ALPHA);
}

TEST_F(EffectParserTest, StatesBlendingSourceDstAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceDstAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::DST_ALPHA);
}

TEST_F(EffectParserTest, StatesBlendingSourceOneMinusDstAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-source/StatesBlendingSourceOneMinusDstAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingSourceFactor(), Blending::Source::ONE_MINUS_DST_ALPHA);
}

/* Blending destination */

TEST_F(EffectParserTest, StatesBlendingDestinationZero)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationZero.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ZERO);
}

TEST_F(EffectParserTest, StatesBlendingDestinationOne)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationOne.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE);
}

TEST_F(EffectParserTest, StatesBlendingDestinationDstColor)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationDstColor.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::DST_COLOR);
}

TEST_F(EffectParserTest, StatesBlendingDestinationOneMinusDstColor)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationOneMinusDstColor.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE_MINUS_DST_COLOR);
}

TEST_F(EffectParserTest, StatesBlendingDestinationSrcAlphaSaturate)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationSrcAlphaSaturate.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::SRC_ALPHA_SATURATE);
}

TEST_F(EffectParserTest, StatesBlendingDestinationOneMinusSrcAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationOneMinusSrcAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE_MINUS_SRC_ALPHA);
}

TEST_F(EffectParserTest, StatesBlendingDestinationDstAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationDstAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::DST_ALPHA);
}

TEST_F(EffectParserTest, StatesBlendingDestinationOneMinusDstAlpha)
{
    auto fx = loadEffect("effect/state/default-value/blending-destination/StatesBlendingDestinationOneMinusDstAlpha.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().blendingDestinationFactor(), Blending::Destination::ONE_MINUS_DST_ALPHA);
}

/* Color mask */

TEST_F(EffectParserTest, StatesColorMaskTrue)
{
    auto fx = loadEffect("effect/state/default-value/color-mask/StatesColorMaskTrue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().colorMask(), true);
}

TEST_F(EffectParserTest, StatesColorMaskFalse)
{
    auto fx = loadEffect("effect/state/default-value/color-mask/StatesColorMaskFalse.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().colorMask(), false);
}

/* Depth mask */

TEST_F(EffectParserTest, StatesDepthMaskTrue)
{
    auto fx = loadEffect("effect/state/default-value/depth-mask/StatesDepthMaskTrue.effect");

    auto value = fx->techniques().at("default")[0]->states().depthMask();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthMask(), true);
}

TEST_F(EffectParserTest, StatesDepthMaskFalse)
{
    auto fx = loadEffect("effect/state/default-value/depth-mask/StatesDepthMaskFalse.effect");

    auto value = fx->techniques().at("default")[0]->states().depthMask();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthMask(), false);
}

/* Depth function */

TEST_F(EffectParserTest, StatesDepthFunctionAlways)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionAlways.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::ALWAYS);
}

TEST_F(EffectParserTest, StatesDepthFunctionEqual)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionGreater)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionGreater.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::GREATER);
}

TEST_F(EffectParserTest, StatesDepthFunctionGreaterEqual)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionGreaterEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::GREATER_EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionLess)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionLess.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::LESS);
}

TEST_F(EffectParserTest, StatesDepthFunctionLessEqual)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionLessEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::LESS_EQUAL);
}

TEST_F(EffectParserTest, StatesDepthFunctionNever)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionNever.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::NEVER);
}

TEST_F(EffectParserTest, StatesDepthFunctionNotEqual)
{
    auto fx = loadEffect("effect/state/default-value/depth-function/StatesDepthFunctionNotEqual.effect");

    auto value = fx->techniques().at("default")[0]->states().depthFunction();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().depthFunction(), CompareMode::NOT_EQUAL);
}

/* Triangle Culling */

TEST_F(EffectParserTest, StatesTriangleCullingBack)
{
    auto fx = loadEffect("effect/state/default-value/triangle-culling/StatesTriangleCullingBack.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::BACK);
}

TEST_F(EffectParserTest, StatesTriangleCullingBoth)
{
    auto fx = loadEffect("effect/state/default-value/triangle-culling/StatesTriangleCullingBoth.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::BOTH);
}

TEST_F(EffectParserTest, StatesTriangleCullingFront)
{
    auto fx = loadEffect("effect/state/default-value/triangle-culling/StatesTriangleCullingFront.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::FRONT);
}

TEST_F(EffectParserTest, StatesTriangleCullingNone)
{
    auto fx = loadEffect("effect/state/default-value/triangle-culling/StatesTriangleCullingNone.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().triangleCulling(), TriangleCulling::NONE);
}

/* Stencil test */

TEST_F(EffectParserTest, StatesStencilTestArrayWithOpsArray)
{
    auto fx = loadEffect("effect/state/default-value/stencil-test/StatesStencilTestArrayWithOpsArray.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::EQUAL);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilReference(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilMask(), 0);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::INCR_WRAP);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::DECR);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::DECR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilTestObjectWithOpsArray)
{
    auto fx = loadEffect("effect/state/default-value/stencil-test/StatesStencilTestObjectWithOpsArray.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::EQUAL);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilReference(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilMask(), 0);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::ZERO);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::REPLACE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::INCR);
}

TEST_F(EffectParserTest, StatesStencilTestArrayWithOpsObject)
{
    auto fx = loadEffect("effect/state/default-value/stencil-test/StatesStencilTestArrayWithOpsObject.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::EQUAL);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilReference(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilMask(), 0);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::ZERO);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::REPLACE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::INCR);
}

TEST_F(EffectParserTest, StatesStencilTestObjectWithOpsObject)
{
    auto fx = loadEffect("effect/state/default-value/stencil-test/StatesStencilTestObjectWithOpsObject.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::EQUAL);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilReference(), 1);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilMask(), 0);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::ZERO);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::REPLACE);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::INCR);
}

/* Stencil function */

TEST_F(EffectParserTest, StatesStencilFunctionAlways)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionAlways.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::ALWAYS);
}

TEST_F(EffectParserTest, StatesStencilFunctionEqual)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionEqual.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::EQUAL);
}

TEST_F(EffectParserTest, StatesStencilFunctionGreater)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionGreater.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::GREATER);
}

TEST_F(EffectParserTest, StatesStencilFunctionGreaterEqual)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionGreaterEqual.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::GREATER_EQUAL);
}

TEST_F(EffectParserTest, StatesStencilFunctionLess)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionLess.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::LESS);
}

TEST_F(EffectParserTest, StatesStencilFunctionLessEqual)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionLessEqual.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::LESS_EQUAL);
}

TEST_F(EffectParserTest, StatesStencilFunctionNever)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionNever.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::NEVER);
}

TEST_F(EffectParserTest, StatesStencilFunctionNotEqual)
{
    auto fx = loadEffect("effect/state/default-value/stencil-function/StatesStencilFunctionNotEqual.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFunction(), CompareMode::NOT_EQUAL);
}

/* Stencil Reference */

TEST_F(EffectParserTest, StatesStencilReference0)
{
    auto fx = loadEffect("effect/state/default-value/stencil-reference/StatesStencilReference0.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilReference(), 0);
}

TEST_F(EffectParserTest, StatesStencilReference1)
{
    auto fx = loadEffect("effect/state/default-value/stencil-reference/StatesStencilReference1.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilReference(), 1);
}

/* Stencil Mask */

TEST_F(EffectParserTest, StatesStencilMask0)
{
    auto fx = loadEffect("effect/state/default-value/stencil-mask/StatesStencilMask0.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilMask(), 0);
}

TEST_F(EffectParserTest, StatesStencilMask1)
{
    auto fx = loadEffect("effect/state/default-value/stencil-mask/StatesStencilMask1.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilMask(), 1);
}

/* Stencil fail operation */

TEST_F(EffectParserTest, StatesStencilFailOperationKeep)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationKeep.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::KEEP);
}

TEST_F(EffectParserTest, StatesStencilFailOperationZero)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationZero.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::ZERO);
}

TEST_F(EffectParserTest, StatesStencilFailOperationReplace)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationReplace.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::REPLACE);
}

TEST_F(EffectParserTest, StatesStencilFailOperationIncr)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationIncr.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::INCR);
}

TEST_F(EffectParserTest, StatesStencilFailOperationIncrWrap)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationIncrWrap.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::INCR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilFailOperationDecr)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationDecr.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::DECR);
}

TEST_F(EffectParserTest, StatesStencilFailOperationDecrWrap)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationDecrWrap.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::DECR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilFailOperationInvert)
{
    auto fx = loadEffect("effect/state/default-value/stencil-fail-operation/StatesStencilFailOperationInvert.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilFailOperation(), StencilOperation::INVERT);
}

/* Stencil Z fail operation */

TEST_F(EffectParserTest, StatesStencilZFailOperationKeep)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationKeep.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::KEEP);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationZero)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationZero.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::ZERO);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationReplace)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationReplace.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::REPLACE);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationIncr)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationIncr.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::INCR);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationIncrWrap)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationIncrWrap.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::INCR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationDecr)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationDecr.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::DECR);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationDecrWrap)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationDecrWrap.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::DECR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilZFailOperationInvert)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-fail-operation/StatesStencilZFailOperationInvert.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZFailOperation(), StencilOperation::INVERT);
}

/* Stencil Z pass operation */

TEST_F(EffectParserTest, StatesStencilZPassOperationKeep)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationKeep.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::KEEP);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationZero)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationZero.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::ZERO);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationReplace)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationReplace.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::REPLACE);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationIncr)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationIncr.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::INCR);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationIncrWrap)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationIncrWrap.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::INCR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationDecr)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationDecr.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::DECR);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationDecrWrap)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationDecrWrap.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::DECR_WRAP);
}

TEST_F(EffectParserTest, StatesStencilZPassOperationInvert)
{
    auto fx = loadEffect("effect/state/default-value/stencil-z-pass-operation/StatesStencilZPassOperationInvert.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().stencilZPassOperation(), StencilOperation::INVERT);
}

/* Scissor test */

TEST_F(EffectParserTest, StatesScissorTestTrue)
{
    auto fx = loadEffect("effect/state/default-value/scissor-test/StatesScissorTestTrue.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().scissorTest(), true);
}

TEST_F(EffectParserTest, StatesScissorTestFalse)
{
    auto fx = loadEffect("effect/state/default-value/scissor-test/StatesScissorTestFalse.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().scissorTest(), false);
}


/* Scissor box */

TEST_F(EffectParserTest, StatesScissorBoxArray)
{
    auto fx = loadEffect("effect/state/default-value/scissor-box/StatesScissorBoxArray.effect");

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().at("default")[0]->states().scissorBox(), math::ivec4(1, 1, 42, 42));
}

/* Target */

TEST_F(EffectParserTest, StatesTargetSize)
{
    auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
    auto fx = MinkoTests::loadEffect("effect/state/default-value/target/StatesTargetSize.effect", assets);
    auto states = fx->techniques().at("default")[0]->states();

    ASSERT_NE(fx, nullptr);
    ASSERT_NE(states.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(states.target(), assets->texture("test-render-target")->sampler());
    ASSERT_NE(assets->texture("test-render-target"), nullptr);
    ASSERT_EQ(assets->texture("test-render-target")->width(), 1024);
    ASSERT_EQ(assets->texture("test-render-target")->height(), 1024);
}

TEST_F(EffectParserTest, StatesTargetWidthHeight)
{
    auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
    auto fx = MinkoTests::loadEffect("effect/state/default-value/target/StatesTargetWidthHeight.effect", assets);
    auto states = fx->techniques().at("default")[0]->states();

    ASSERT_NE(fx, nullptr);
    ASSERT_NE(states.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(states.target(), assets->texture("test-render-target")->sampler());
    ASSERT_NE(assets->texture("test-render-target"), nullptr);
    ASSERT_EQ(assets->texture("test-render-target")->width(), 2048);
    ASSERT_EQ(assets->texture("test-render-target")->height(), 1024);
}

/***********************/
/*** States bindings ***/
/***********************/

TEST_F(EffectParserTest, StatesBindingPriority)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingPriority.effect";

    checkStateBinding(filename, States::PROPERTY_PRIORITY);
}

TEST_F(EffectParserTest, StatesBindingZSorted)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingZSorted.effect";

    checkStateBinding(filename, States::PROPERTY_ZSORTED);
}

TEST_F(EffectParserTest, StatesBindingBlendingSource)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingBlendingSource.effect";

    checkStateBinding(filename, States::PROPERTY_BLENDING_SOURCE);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestination)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingBlendingDestination.effect";

    checkStateBinding(filename, States::PROPERTY_BLENDING_DESTINATION);
}

TEST_F(EffectParserTest, StatesBindingColorMask)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingColorMask.effect";

    checkStateBinding(filename, States::PROPERTY_COLOR_MASK);
}

TEST_F(EffectParserTest, StatesBindingDepthMask)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingDepthMask.effect";

    checkStateBinding(filename, States::PROPERTY_DEPTH_MASK);
}

TEST_F(EffectParserTest, StatesBindingDepthFunction)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingDepthFunction.effect";

    checkStateBinding(filename, States::PROPERTY_DEPTH_FUNCTION);
}

TEST_F(EffectParserTest, StatesBindingTriangleCulling)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingTriangleCulling.effect";

    checkStateBinding(filename, States::PROPERTY_TRIANGLE_CULLING);
}

TEST_F(EffectParserTest, StatesBindingStencilFunction)
{
    auto filename = "effect/state/binding/no-default-value/stencil/StatesBindingStencilFunction.effect";

    checkStateBinding(filename, States::PROPERTY_STENCIL_FUNCTION);
}

TEST_F(EffectParserTest, StatesBindingStencilReference)
{
    auto filename = "effect/state/binding/no-default-value/stencil/StatesBindingStencilReference.effect";

    checkStateBinding(filename, States::PROPERTY_STENCIL_REFERENCE);
}

TEST_F(EffectParserTest, StatesBindingStencilMask)
{
    auto filename = "effect/state/binding/no-default-value/stencil/StatesBindingStencilMask.effect";

    checkStateBinding(filename, States::PROPERTY_STENCIL_MASK);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperation)
{
    auto filename = "effect/state/binding/no-default-value/stencil/StatesBindingStencilFailOperation.effect";

    checkStateBinding(filename, States::PROPERTY_STENCIL_FAIL_OPERATION);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperation)
{
    auto filename = "effect/state/binding/no-default-value/stencil/StatesBindingStencilZFailOperation.effect";

    checkStateBinding(filename, States::PROPERTY_STENCIL_ZFAIL_OPERATION);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperation)
{
    auto filename = "effect/state/binding/no-default-value/stencil/StatesBindingStencilZPassOperation.effect";

    checkStateBinding(filename, States::PROPERTY_STENCIL_ZPASS_OPERATION);
}

TEST_F(EffectParserTest, StatesBindingScissorTest)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingScissorTest.effect";

    checkStateBinding(filename, States::PROPERTY_SCISSOR_TEST);
}

TEST_F(EffectParserTest, StatesBindingScissorBox)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingScissorBox.effect";

    checkStateBinding(filename, States::PROPERTY_SCISSOR_BOX);
}

TEST_F(EffectParserTest, StatesBindingTarget)
{
    auto filename = "effect/state/binding/no-default-value/StatesBindingTarget.effect";

    checkStateBinding(filename, States::PROPERTY_TARGET);
}

/** States binding with default value **/

/* Priority */

TEST_F(EffectParserTest, StatesBindingPriorityWithDefaultValueNumber)
{
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueNumber.effect";
    auto defaultValue = 42.f;

    checkStateBindingWithDefaultValue<float>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingPriorityWithDefaultValueArray)
{
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueArray.effect";
    auto defaultValue = 4042.f;

    checkStateBindingWithDefaultValue<float>(effectFile, stateName, defaultValue);
}

/* ZSorted */

TEST_F(EffectParserTest, StatesBindingZSortedWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_ZSORTED;
    auto effectFile = "effect/state/binding/with-default-value/zsorted/StatesBindingZSortedWithDefaultValueTrue.effect";
    auto defaultValue = true;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingZSortedWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_ZSORTED;
    auto effectFile = "effect/state/binding/with-default-value/zsorted/StatesBindingZSortedWithDefaultValueFalse.effect";
    auto defaultValue = false;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

// Don't forget there is no binding for blending modebl

/* Blending Source */

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueZero.effect";
    auto defaultValue = Blending::Source::ZERO;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueOne)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOne.effect";
    auto defaultValue = Blending::Source::ONE;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueSrcColor)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueSrcColor.effect";
    auto defaultValue = Blending::Source::SRC_COLOR;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueOneMinusSrcColor)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOneMinusSrcColor.effect";
    auto defaultValue = Blending::Source::ONE_MINUS_SRC_COLOR;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueSrcAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueSrcAlpha.effect";
    auto defaultValue = Blending::Source::SRC_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueOneMinusSrcAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOneMinusSrcAlpha.effect";
    auto defaultValue = Blending::Source::ONE_MINUS_SRC_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueDstAlpha.effect";
    auto defaultValue = Blending::Source::DST_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingSourceWithDefaultValueOneMinusDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOneMinusDstAlpha.effect";
    auto defaultValue = Blending::Source::ONE_MINUS_DST_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Source>(effectFile, stateName, defaultValue);
}

/* Blending destination */

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueZero.effect";
    auto defaultValue = Blending::Destination::ZERO;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueOne)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOne.effect";
    auto defaultValue = Blending::Destination::ONE;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueDstColor)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueDstColor.effect";
    auto defaultValue = Blending::Destination::DST_COLOR;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueOneMinusDstColor)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOneMinusDstColor.effect";
    auto defaultValue = Blending::Destination::ONE_MINUS_DST_COLOR;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueSrcAlphaSaturate)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueSrcAlphaSaturate.effect";
    auto defaultValue = Blending::Destination::SRC_ALPHA_SATURATE;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueOneMinusSrcAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOneMinusSrcAlpha.effect";
    auto defaultValue = Blending::Destination::ONE_MINUS_SRC_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueDstAlpha.effect";
    auto defaultValue = Blending::Destination::DST_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingBlendingDestinationWithDefaultValueOneMinusDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOneMinusDstAlpha.effect";
    auto defaultValue = Blending::Destination::ONE_MINUS_DST_ALPHA;

    checkStateBindingWithDefaultValue<Blending::Destination>(effectFile, stateName, defaultValue);
}

/* Color mask */

TEST_F(EffectParserTest, StatesBindingColorMaskWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_COLOR_MASK;
    auto effectFile = "effect/state/binding/with-default-value/color-mask/StatesBindingColorMaskWithDefaultValueTrue.effect";
    auto defaultValue = true;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingColorMaskWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_COLOR_MASK;
    auto effectFile = "effect/state/binding/with-default-value/color-mask/StatesBindingColorMaskWithDefaultValueFalse.effect";
    auto defaultValue = false;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

/* Depth Mask */

TEST_F(EffectParserTest, StatesBindingDepthMaskWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_DEPTH_MASK;
    auto effectFile = "effect/state/binding/with-default-value/depth-mask/StatesBindingDepthMaskWithDefaultValueTrue.effect";
    auto defaultValue = true;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthMaskWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_DEPTH_MASK;
    auto effectFile = "effect/state/binding/with-default-value/depth-mask/StatesBindingDepthMaskWithDefaultValueFalse.effect";
    auto defaultValue = false;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

/* Depth Function */

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueAlways)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueAlways.effect";
    auto defaultValue = CompareMode::ALWAYS;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueEqual.effect";
    auto defaultValue = CompareMode::EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueGreater)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueGreater.effect";
    auto defaultValue = CompareMode::GREATER;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueGreaterEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueGreaterEqual.effect";
    auto defaultValue = CompareMode::GREATER_EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueLess)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueLess.effect";
    auto defaultValue = CompareMode::LESS;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueLessEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueLessEqual.effect";
    auto defaultValue = CompareMode::LESS_EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueNever)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueNever.effect";
    auto defaultValue = CompareMode::NEVER;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingDepthFunctionWithDefaultValueNotEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueNotEqual.effect";
    auto defaultValue = CompareMode::NOT_EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

/* Triangle culling */

TEST_F(EffectParserTest, StatesBindingTriangleCullingWithDefaultValueNone)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueNone.effect";
    auto defaultValue = TriangleCulling::NONE;

    checkStateBindingWithDefaultValue<TriangleCulling>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingTriangleCullingWithDefaultValueFront)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueFront.effect";
    auto defaultValue = TriangleCulling::FRONT;

    checkStateBindingWithDefaultValue<TriangleCulling>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingTriangleCullingWithDefaultValueBack)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueBack.effect";
    auto defaultValue = TriangleCulling::BACK;

    checkStateBindingWithDefaultValue<TriangleCulling>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingTriangleCullingWithDefaultValueBoth)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueBoth.effect";
    auto defaultValue = TriangleCulling::BOTH;

    checkStateBindingWithDefaultValue<TriangleCulling>(effectFile, stateName, defaultValue);
}

/* Stencil Function */

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueAlways)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueAlways.effect";
    auto defaultValue = CompareMode::ALWAYS;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueEqual.effect";
    auto defaultValue = CompareMode::EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueGreater)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueGreater.effect";
    auto defaultValue = CompareMode::GREATER;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueGreaterEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueGreaterEqual.effect";
    auto defaultValue = CompareMode::GREATER_EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueLess)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueLess.effect";
    auto defaultValue = CompareMode::LESS;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueLessEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueLessEqual.effect";
    auto defaultValue = CompareMode::LESS_EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueNever)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueNever.effect";
    auto defaultValue = CompareMode::NEVER;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFunctionWithDefaultValueNotEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueNotEqual.effect";
    auto defaultValue = CompareMode::NOT_EQUAL;

    checkStateBindingWithDefaultValue<CompareMode>(effectFile, stateName, defaultValue);
}

/* Stencil reference */

TEST_F(EffectParserTest, StatesBindingStencilReferenceWithDefaultValue0)
{
    auto stateName = States::PROPERTY_STENCIL_REFERENCE;
    auto effectFile = "effect/state/binding/with-default-value/stencil-reference/StatesBindingStencilReferenceWithDefaultValue0.effect";
    auto defaultValue = 0;

    checkStateBindingWithDefaultValue<int>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilReferenceWithDefaultValue1)
{
    auto stateName = States::PROPERTY_STENCIL_REFERENCE;
    auto effectFile = "effect/state/binding/with-default-value/stencil-reference/StatesBindingStencilReferenceWithDefaultValue1.effect";
    auto defaultValue = 1;

    checkStateBindingWithDefaultValue<int>(effectFile, stateName, defaultValue);
}

/* Stencil mask */

TEST_F(EffectParserTest, StatesBindingStencilMaskWithDefaultValue0)
{
    auto stateName = States::PROPERTY_STENCIL_MASK;
    auto effectFile = "effect/state/binding/with-default-value/stencil-mask/StatesBindingStencilMaskWithDefaultValue0.effect";
    auto defaultValue = 0;

    checkStateBindingWithDefaultValue<uint>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilMaskWithDefaultValue1)
{
    auto stateName = States::PROPERTY_STENCIL_MASK;
    auto effectFile = "effect/state/binding/with-default-value/stencil-mask/StatesBindingStencilMaskWithDefaultValue1.effect";
    auto defaultValue = 1;

    checkStateBindingWithDefaultValue<uint>(effectFile, stateName, defaultValue);
}

/* Stencil fail operation */

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueKeep)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueKeep.effect";
    auto defaultValue = StencilOperation::KEEP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueZero.effect";
    auto defaultValue = StencilOperation::ZERO;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueReplace)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueReplace.effect";
    auto defaultValue = StencilOperation::REPLACE;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueIncr)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueIncr.effect";
    auto defaultValue = StencilOperation::INCR;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueIncrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueIncrWrap.effect";
    auto defaultValue = StencilOperation::INCR_WRAP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueDecr)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueDecr.effect";
    auto defaultValue = StencilOperation::DECR;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueDecrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueDecrWrap.effect";
    auto defaultValue = StencilOperation::DECR_WRAP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilFailOperationWithDefaultValueInvert)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueInvert.effect";
    auto defaultValue = StencilOperation::INVERT;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

/* Stencil Z fail operation */

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueKeep)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueKeep.effect";
    auto defaultValue = StencilOperation::KEEP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueZero.effect";
    auto defaultValue = StencilOperation::ZERO;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueReplace)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueReplace.effect";
    auto defaultValue = StencilOperation::REPLACE;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueIncr)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueIncr.effect";
    auto defaultValue = StencilOperation::INCR;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueIncrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueIncrWrap.effect";
    auto defaultValue = StencilOperation::INCR_WRAP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueDecr)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueDecr.effect";
    auto defaultValue = StencilOperation::DECR;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueDecrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueDecrWrap.effect";
    auto defaultValue = StencilOperation::DECR_WRAP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZFailOperationWithDefaultValueInvert)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueInvert.effect";
    auto defaultValue = StencilOperation::INVERT;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

/* Stencil Z pass operation */

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueKeep)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueKeep.effect";
    auto defaultValue = StencilOperation::KEEP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueZero.effect";
    auto defaultValue = StencilOperation::ZERO;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueReplace)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueReplace.effect";
    auto defaultValue = StencilOperation::REPLACE;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueIncr)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueIncr.effect";
    auto defaultValue = StencilOperation::INCR;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueIncrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueIncrWrap.effect";
    auto defaultValue = StencilOperation::INCR_WRAP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueDecr)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueDecr.effect";
    auto defaultValue = StencilOperation::DECR;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueDecrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueDecrWrap.effect";
    auto defaultValue = StencilOperation::DECR_WRAP;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingStencilZPassOperationWithDefaultValueInvert)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueInvert.effect";
    auto defaultValue = StencilOperation::INVERT;

    checkStateBindingWithDefaultValue<StencilOperation>(effectFile, stateName, defaultValue);
}

/* Scissor test */

TEST_F(EffectParserTest, StatesBindingScissorTestWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_SCISSOR_TEST;
    auto effectFile = "effect/state/binding/with-default-value/scissor-test/StatesBindingScissorTestWithDefaultValueTrue.effect";
    auto defaultValue = true;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

TEST_F(EffectParserTest, StatesBindingScissorTestWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_SCISSOR_TEST;
    auto effectFile = "effect/state/binding/with-default-value/scissor-test/StatesBindingScissorTestWithDefaultValueFalse.effect";
    auto defaultValue = false;

    checkStateBindingWithDefaultValue<bool>(effectFile, stateName, defaultValue);
}

/* Scissor box */

TEST_F(EffectParserTest, StatesBindingScissorBoxWithDefaultValueArray)
{
    auto stateName = States::PROPERTY_SCISSOR_BOX;
    auto effectFile = "effect/state/binding/with-default-value/scissor-box/StatesBindingScissorBoxWithDefaultValueArray.effect";
    auto defaultValue = math::ivec4(1, 1, 42, 42);

    checkStateBindingWithDefaultValue<math::ivec4>(effectFile, stateName, defaultValue);
}

/* Target */

TEST_F(EffectParserTest, StatesBindingTargetWithDefaultValueSize)
{
    auto stateName = States::PROPERTY_TARGET;
    auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
    auto fx = MinkoTests::loadEffect("effect/state/binding/with-default-value/target/StatesBindingTargetWithDefaultValueSize.effect", assets);
    auto states = fx->techniques().at("default")[0]->states();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto stateBindings = fx->techniques().at("default")[0]->stateBindings();

    ASSERT_EQ(stateBindings.bindings.size(), 1);
    ASSERT_EQ(stateBindings.bindings.at(stateName).propertyName, "material[${materialUuid}]." + stateName);
    ASSERT_EQ(stateBindings.bindings.at(stateName).source, data::Binding::Source::TARGET);

    ASSERT_NE(states.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(states.target(), assets->texture("test-render-target")->sampler());
    ASSERT_NE(assets->texture("test-render-target"), nullptr);
    ASSERT_EQ(assets->texture("test-render-target")->width(), 1024);
    ASSERT_EQ(assets->texture("test-render-target")->height(), 1024);
}

TEST_F(EffectParserTest, StatesBindingTargetWithDefaultValueWidthHeight)
{
    auto stateName = States::PROPERTY_TARGET;
    auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
    auto fx = MinkoTests::loadEffect("effect/state/binding/with-default-value/target/StatesBindingTargetWithDefaultValueWidthHeight.effect", assets);
    auto states = fx->techniques().at("default")[0]->states();

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 1);
    ASSERT_EQ(fx->techniques().at("default").size(), 1);

    auto stateBindings = fx->techniques().at("default")[0]->stateBindings();

    ASSERT_EQ(stateBindings.bindings.size(), 1);
    ASSERT_EQ(stateBindings.bindings.at(stateName).propertyName, "material[${materialUuid}]." + stateName);
    ASSERT_EQ(stateBindings.bindings.at(stateName).source, data::Binding::Source::TARGET);

    ASSERT_NE(states.target(), States::DEFAULT_TARGET);
    ASSERT_EQ(states.target(), assets->texture("test-render-target")->sampler());
    ASSERT_NE(assets->texture("test-render-target"), nullptr);
    ASSERT_EQ(assets->texture("test-render-target")->width(), 2048);
    ASSERT_EQ(assets->texture("test-render-target")->height(), 1024);
}

/**********************/
/*** Sampler states ***/
/**********************/

/** Sampler states binding without default value **/

/* Wrap mode */

TEST_F(EffectParserTest, SamplerStatesWrapModeClamp)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesWrapModeClamp.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;
    auto defaultValue = WrapMode::CLAMP;

    checkSamplerStateDefaultValue<WrapMode>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesWrapModeRepeat)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesWrapModeRepeat.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;
    auto defaultValue = WrapMode::REPEAT;

    checkSamplerStateDefaultValue<WrapMode>(filename, samplerStateProperty, defaultValue);
}

/* Texture filter */

TEST_F(EffectParserTest, SamplerStatesTextureFilterLinear)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesTextureFilterLinear.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;
    auto defaultValue = TextureFilter::LINEAR;

    checkSamplerStateDefaultValue<TextureFilter>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesTextureFilterNearest)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesTextureFilterNearest.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;
    auto defaultValue = TextureFilter::NEAREST;

    checkSamplerStateDefaultValue<TextureFilter>(filename, samplerStateProperty, defaultValue);
}

/* Mip filter */

TEST_F(EffectParserTest, SamplerStatesMipFilterNone)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesMipFilterNone.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto defaultValue = MipFilter::NONE;

    checkSamplerStateDefaultValue<MipFilter>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesMipFilterLinear)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesMipFilterLinear.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto defaultValue = MipFilter::LINEAR;

    checkSamplerStateDefaultValue<MipFilter>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesMipFilterNearest)
{
    auto filename = "effect/sampler-state/default-value/SamplerStatesMipFilterNearest.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto defaultValue = MipFilter::NEAREST;

    checkSamplerStateDefaultValue<MipFilter>(filename, samplerStateProperty, defaultValue);
}

/** Sampler states binding with default value **/

/* Wrap mode */

TEST_F(EffectParserTest, SamplerStatesBindingWrapMode)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingWrapMode.effect";

    checkSamplerStateBinding(filename, SamplerStates::PROPERTY_WRAP_MODE);
}

/* Texture filter */

TEST_F(EffectParserTest, SamplerStatesBindingTextureFilter)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingTextureFilter.effect";

    checkSamplerStateBinding(filename, SamplerStates::PROPERTY_TEXTURE_FILTER);
}

TEST_F(EffectParserTest, SamplerStatesBindingMipFilter)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingMipFilter.effect";

    checkSamplerStateBinding(filename, SamplerStates::PROPERTY_MIP_FILTER);
}

/** Sampler states binding with default value **/

/* Wrap mode */

TEST_F(EffectParserTest, SamplerStatesBindingWrapModeWithDefaultValueClamp)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingWrapModeWithDefaultValueClamp.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;
    auto defaultValue = WrapMode::CLAMP;

    checkSamplerStateBindingWithDefaultValue<WrapMode>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesBindingWrapModeWithDefaultValueRepeat)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingWrapModeWithDefaultValueRepeat.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;
    auto defaultValue = WrapMode::REPEAT;

    checkSamplerStateBindingWithDefaultValue<WrapMode>(filename, samplerStateProperty, defaultValue);
}

/* Texture filter */

TEST_F(EffectParserTest, SamplerStatesBindingTextureFilterWithDefaultValueLinear)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingTextureFilterWithDefaultValueLinear.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;
    auto defaultValue = TextureFilter::LINEAR;

    checkSamplerStateBindingWithDefaultValue<TextureFilter>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesBindingTextureFilterWithDefaultValueNearest)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingTextureFilterWithDefaultValueNearest.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;
    auto defaultValue = TextureFilter::NEAREST;

    checkSamplerStateBindingWithDefaultValue<TextureFilter>(filename, samplerStateProperty, defaultValue);
}

/* Mip filter */

TEST_F(EffectParserTest, SamplerStatesBindingMipFilterWithDefaultValueLinear)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingMipFilterWithDefaultValueLinear.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto defaultValue = MipFilter::LINEAR;

    checkSamplerStateBindingWithDefaultValue<MipFilter>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesBindingMipFilterWithDefaultValueNearest)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingMipFilterWithDefaultValueNearest.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto defaultValue = MipFilter::NEAREST;

    checkSamplerStateBindingWithDefaultValue<MipFilter>(filename, samplerStateProperty, defaultValue);
}

TEST_F(EffectParserTest, SamplerStatesBindingMipFilterWithDefaultValueNone)
{
    auto filename = "effect/sampler-state/binding/SamplerStatesBindingMipFilterWithDefaultValueNone.effect";
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;
    auto defaultValue = MipFilter::NONE;

    checkSamplerStateBindingWithDefaultValue<MipFilter>(filename, samplerStateProperty, defaultValue);
}
