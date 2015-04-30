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

#include "DrawCallPoolTest.hpp"

#include "minko/Hash.hpp"

using namespace minko;
using namespace minko::render;

/** Functions to factor tests **/

template<typename T>
DrawCall*
createDrawCallWithState(const std::string& effectFile,
                        const std::string& stateName,
                        T stateMaterialValue,
                        material::Material::Ptr material,
                        data::Store& targetData)
{
    auto fx = MinkoTests::loadEffect(effectFile);
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;

    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });
    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    return pool.drawCalls().front();
}


TEST_F(DrawCallPoolTest, Constructor)
{
    DrawCallPool drawCallPool;
}

TEST_F(DrawCallPoolTest, UniformDefaultToBindingSwap)
{
    auto fx = MinkoTests::loadEffect("effect/uniform/binding/OneUniformBindingAndDefault.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    render::EffectVariables variables = { { "geometryUuid", geom->uuid() } };

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(pool.drawCalls().front()->boundFloatUniforms().size(), 1);
    ASSERT_EQ(
        pool.drawCalls().front()->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );

    auto p = data::Provider::create();

    p->set("diffuseColor", math::vec4(1.));
    targetData.addProvider(p);
    pool.update();

    ASSERT_EQ(pool.drawCalls().front()->boundFloatUniforms().size(), 1);
    ASSERT_NE(
        pool.drawCalls().front()->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
    ASSERT_EQ(
        pool.drawCalls().front()->boundFloatUniforms()[0].data,
        math::value_ptr(targetData.get<math::vec4>("diffuseColor"))
    );
}

TEST_F(DrawCallPoolTest, UniformBindingToDefaultSwap)
{
    auto fx = MinkoTests::loadEffect("effect/uniform/binding/OneUniformBindingAndDefault.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    render::EffectVariables variables = { { "geometryUuid", geom->uuid() } };

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    auto p = data::Provider::create();

    p->set("diffuseColor", math::vec4(1.));
    targetData.addProvider(p);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(pool.drawCalls().front()->boundFloatUniforms().size(), 1);
    ASSERT_NE(
        pool.drawCalls().front()->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
    ASSERT_EQ(
        pool.drawCalls().front()->boundFloatUniforms()[0].data,
        math::value_ptr(targetData.get<math::vec4>("diffuseColor"))
    );

    p->unset("diffuseColor");
    pool.update();

    ASSERT_EQ(pool.drawCalls().front()->boundFloatUniforms().size(), 1);
    ASSERT_EQ(
        pool.drawCalls().front()->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
}

TEST_F(DrawCallPoolTest, WatchAndDefineIntMacro)
{
    auto fx = MinkoTests::loadEffect("effect/macro/binding/OneIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    render::EffectVariables variables;

    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().bindings.size(), 1);
    ASSERT_EQ(targetData.propertyChanged("bar").numCallbacks(), 0);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("bar").numCallbacks(), 1);
    ASSERT_FALSE(pool.drawCalls().front()->program()->definedMacroNames().find("FOO") != pool.drawCalls().front()->program()->definedMacroNames().end());

    auto p = data::Provider::create();
    p->set("bar", 42);
    targetData.addProvider(p);
    pool.update();

    ASSERT_TRUE(pool.drawCalls().front()->program()->definedMacroNames().find("FOO") != pool.drawCalls().front()->program()->definedMacroNames().end());

    p->unset("bar");
    pool.update();

    ASSERT_FALSE(pool.drawCalls().front()->program()->definedMacroNames().find("FOO") != pool.drawCalls().front()->program()->definedMacroNames().end());
}

TEST_F(DrawCallPoolTest, WatchAndDefineVariableIntMacro)
{
    auto fx = MinkoTests::loadEffect("effect/macro/binding/OneVariableIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto p = data::Provider::create();
    std::string materialUuid = p->uuid();
    render::EffectVariables variables = {{ "materialUuid", materialUuid }};

    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().bindings.size(), 1);
    ASSERT_EQ(targetData.propertyChanged("bar").numCallbacks(), 0);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid + "].bar").numCallbacks(), 1);
    ASSERT_FALSE(pool.drawCalls().front()->program()->definedMacroNames().find("FOO") != pool.drawCalls().front()->program()->definedMacroNames().end());

    p->set("bar", 42);
    targetData.addProvider(p, "material");
    pool.update();

    ASSERT_TRUE(pool.drawCalls().front()->program()->definedMacroNames().find("FOO") != pool.drawCalls().front()->program()->definedMacroNames().end());

    p->unset("bar");
    pool.update();

    ASSERT_FALSE(pool.drawCalls().front()->program()->definedMacroNames().find("FOO") != pool.drawCalls().front()->program()->definedMacroNames().end());
}

TEST_F(DrawCallPoolTest, StopWatchingMacroAfterDrawCallsRemoved)
{
    auto fx = MinkoTests::loadEffect("effect/macro/binding/OneVariableIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto p = data::Provider::create();
    std::string materialUuid = p->uuid();
    render::EffectVariables variables = {{ "materialUuid", materialUuid }};

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid + "].bar").numCallbacks(), 1);

    pool.removeDrawCalls(drawCalls);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid + "].bar").numCallbacks(), 0);
}

TEST_F(DrawCallPoolTest, SameMacroBindingDifferentVariables)
{
    auto fx = MinkoTests::loadEffect("effect/macro/binding/OneVariableIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto p1 = data::Provider::create();
    auto p2 = data::Provider::create();
    std::string materialUuid1 = p1->uuid();
    std::string materialUuid2 = p2->uuid();
    render::EffectVariables variables1 = { { "materialUuid", materialUuid1 } };
    render::EffectVariables variables2 = { { "materialUuid", materialUuid2 } };

    targetData.addProvider(p1, component::Surface::MATERIAL_COLLECTION_NAME);
    targetData.addProvider(p2, component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables1, rootData, rendererData, targetData);
    auto drawCalls2 = pool.addDrawCalls(fx, "default", variables2, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid1 + "].bar").numCallbacks(), 1);
    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid2 + "].bar").numCallbacks(), 1);
}

/** Sampler states binding swap **/

TEST_F(DrawCallPoolTest, SamplerStateSwapWrapModeBindingToDefaultClamp)
{
    auto samplerStateMaterialValue = WrapMode::REPEAT;
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingWrapModeWithDefaultValueClamp.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, material->data()->get<WrapMode>(samplerStateBindingName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);
    pool.update();

    ASSERT_EQ(*sampler.wrapMode, pass->uniformBindings().defaultValues.get<WrapMode>(sampleStateUniformName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapWrapModeBindingToDefaultRepeat)
{
    auto samplerStateMaterialValue = WrapMode::CLAMP;
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingWrapModeWithDefaultValueClamp.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, material->data()->get<WrapMode>(samplerStateBindingName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);
    pool.update();

    ASSERT_EQ(*sampler.wrapMode, pass->uniformBindings().defaultValues.get<WrapMode>(sampleStateUniformName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapTextureFilterBindingToDefaultLinear)
{
    auto samplerStateMaterialValue = TextureFilter::NEAREST;
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingTextureFilterWithDefaultValueLinear.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, material->data()->get<TextureFilter>(samplerStateBindingName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);
    pool.update();

    auto value = pass->uniformBindings().defaultValues.get<TextureFilter>(sampleStateUniformName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, pass->uniformBindings().defaultValues.get<TextureFilter>(sampleStateUniformName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapTextureFilterBindingToDefaultNearest)
{
    auto samplerStateMaterialValue = TextureFilter::LINEAR;
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingTextureFilterWithDefaultValueNearest.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    auto p = data::Provider::create();
    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    auto value = material->data()->get<TextureFilter>(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, material->data()->get<TextureFilter>(samplerStateBindingName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);
    pool.update();

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, pass->uniformBindings().defaultValues.get<TextureFilter>(sampleStateUniformName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapMipFilterBindingToDefaultNone)
{
    auto samplerStateMaterialValue = MipFilter::LINEAR;
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingMipFilterWithDefaultValueNone.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, material->data()->get<MipFilter>(samplerStateBindingName));

    material->data()->unset(samplerStateBindingName);
    pool.update();

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, pass->uniformBindings().defaultValues.get<MipFilter>(sampleStateUniformName));
}

TEST_F(DrawCallPoolTest, SamplerStateSwapMipFilterBindingToDefaultLinear)
{
    auto samplerStateMaterialValue = MipFilter::NONE;
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingMipFilterWithDefaultValueLinear.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, material->data()->get<MipFilter>(samplerStateBindingName));

    material->data()->unset(samplerStateBindingName);
    pool.update();

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, pass->uniformBindings().defaultValues.get<MipFilter>(sampleStateUniformName));
}

TEST_F(DrawCallPoolTest, SamplerStateSwapMipFilterBindingToDefaultNearest)
{
    auto samplerStateMaterialValue = MipFilter::NONE;
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingMipFilterWithDefaultValueNearest.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, material->data()->get<MipFilter>(samplerStateBindingName));

    material->data()->unset(samplerStateBindingName);
    pool.update();

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, pass->uniformBindings().defaultValues.get<MipFilter>(sampleStateUniformName));
}

/** Sampler states binding with no binding value and no default value **/

TEST_F(DrawCallPoolTest, SamplerStatesBindingWrapModeNoDefaultValue)
{
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingWrapModeNoDefaultValue.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_FALSE(pass->uniformBindings().defaultValues.hasProperty(sampleStateUniformName));

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateTextureFilterBindingNoDefaultValue)
{
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingTextureFilterNoDefaultValue.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_FALSE(pass->uniformBindings().defaultValues.hasProperty(sampleStateUniformName));

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStatesBindingMipFilterNoDefaultValue)
{
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/sampler-state/binding/SamplerStatesBindingMipFilterNoDefaultValue.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;

    auto samplerUniformName = "uDiffuseMap";
    auto samplerBindingName = "diffuseMap";

    auto sampleStateUniformName = SamplerStates::uniformNameToSamplerStateName(
        samplerUniformName,
        samplerStateProperty
    );

    auto samplerStateBindingName = SamplerStates::uniformNameToSamplerStateBindingName(
        samplerBindingName,
        samplerStateProperty
    );

    auto material = material::Material::create();
    auto texture = Texture::create(MinkoTests::canvas()->context(), 1024, 1024, false, true);
    render::EffectVariables variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.push_back({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = pool.drawCalls().front()->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_FALSE(pass->uniformBindings().defaultValues.hasProperty(sampleStateUniformName));

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

/** States **/

/* Priority */

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueFirst)
{
    auto stateMaterialValue = 0.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueFirst.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueBackground)
{
    auto stateMaterialValue = 0.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueBackground.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueOpaque)
{
    auto stateMaterialValue = 0.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueOpaque.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueTransparent)
{
    auto stateMaterialValue = 0.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueTransparent.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueLast)
{
    auto stateMaterialValue = 42.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueLast.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueNumber)
{
    auto stateMaterialValue = 0.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueNumber.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingPriorityWithDefaultValueArray)
{
    auto stateMaterialValue = 0.f;
    auto stateName = States::PROPERTY_PRIORITY;
    auto effectFile = "effect/state/binding/with-default-value/priority/StatesBindingPriorityWithDefaultValueArray.effect";

    testStateBindingToDefaultValueSwap<float>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->priority(); }
    );
}

/* ZSorted */

TEST_F(DrawCallPoolTest, StatesBindingZSortedWithDefaultValueFalse)
{
    auto stateMaterialValue = true;
    auto stateName = States::PROPERTY_ZSORTED;
    auto effectFile = "effect/state/binding/with-default-value/zsorted/StatesBindingZSortedWithDefaultValueFalse.effect";

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->zSorted(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingZSortedWithDefaultValueTrue)
{
    auto stateMaterialValue = false;
    auto stateName = States::PROPERTY_ZSORTED;
    auto effectFile = "effect/state/binding/with-default-value/zsorted/StatesBindingZSortedWithDefaultValueTrue.effect";

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->zSorted(); }
    );
}

/* Blending source */

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueZero)
{
    auto stateMaterialValue = Blending::Source::ONE;
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueZero.effect";

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueOne)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOne.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueSrcColor)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueSrcColor.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueOneMinusSrcColor)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOneMinusSrcColor.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueSrcAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueSrcAlpha.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueOneMinusSrcAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOneMinusSrcAlpha.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueDstAlpha.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingSourceWithDefaultValueOneMinusDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_SOURCE;
    auto effectFile = "effect/state/binding/with-default-value/blending-source/StatesBindingBlendingSourceWithDefaultValueOneMinusDstAlpha.effect";
    auto stateMaterialValue = Blending::Source::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Source>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingSource(); }
    );
}

/* Blending destination */

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueZero.effect";
    auto stateMaterialValue = Blending::Destination::ONE;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueOne)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOne.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueDstColor)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueDstColor.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueOneMinusDstColor)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOneMinusDstColor.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueSrcAlphaSaturate)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueSrcAlphaSaturate.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueOneMinusSrcAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOneMinusSrcAlpha.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueDstAlpha.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingBlendingDestinationWithDefaultValueOneMinusDstAlpha)
{
    auto stateName = States::PROPERTY_BLENDING_DESTINATION;
    auto effectFile = "effect/state/binding/with-default-value/blending-destination/StatesBindingBlendingDestinationWithDefaultValueOneMinusDstAlpha.effect";
    auto stateMaterialValue = Blending::Destination::ZERO;

    testStateBindingToDefaultValueSwap<Blending::Destination>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->blendingDestination(); }
    );
}

/* Color mask */

TEST_F(DrawCallPoolTest, StatesBindingColorMaskWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_COLOR_MASK;
    auto effectFile = "effect/state/binding/with-default-value/color-mask/StatesBindingColorMaskWithDefaultValueTrue.effect";
    auto stateMaterialValue = false;

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->colorMask(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingColorMaskWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_COLOR_MASK;
    auto effectFile = "effect/state/binding/with-default-value/color-mask/StatesBindingColorMaskWithDefaultValueFalse.effect";
    auto stateMaterialValue = true;

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->colorMask(); }
    );
}

/* Depth Mask */

TEST_F(DrawCallPoolTest, StatesBindingDepthMaskWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_DEPTH_MASK;
    auto effectFile = "effect/state/binding/with-default-value/depth-mask/StatesBindingDepthMaskWithDefaultValueTrue.effect";
    auto stateMaterialValue = false;

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthMask(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthMaskWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_DEPTH_MASK;
    auto effectFile = "effect/state/binding/with-default-value/depth-mask/StatesBindingDepthMaskWithDefaultValueFalse.effect";
    auto stateMaterialValue = true;

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthMask(); }
    );
}

/* Depth Function */

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueAlways)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueAlways.effect";
    auto stateMaterialValue = CompareMode::EQUAL;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueGreater)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueGreater.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueGreaterEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueGreaterEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueLess)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueLess.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueLessEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueLessEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueNever)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueNever.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingDepthFunctionWithDefaultValueNotEqual)
{
    auto stateName = States::PROPERTY_DEPTH_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/depth-function/StatesBindingDepthFunctionWithDefaultValueNotEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->depthFunction(); }
    );
}

/* Triangle culling */

TEST_F(DrawCallPoolTest, StatesBindingTriangleCullingWithDefaultValueNone)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueNone.effect";
    auto stateMaterialValue = TriangleCulling::FRONT;

    testStateBindingToDefaultValueSwap<TriangleCulling>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->triangleCulling(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingTriangleCullingWithDefaultValueFront)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueFront.effect";
    auto stateMaterialValue = TriangleCulling::NONE;

    testStateBindingToDefaultValueSwap<TriangleCulling>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->triangleCulling(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingTriangleCullingWithDefaultValueBack)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueBack.effect";
    auto stateMaterialValue = TriangleCulling::NONE;

    testStateBindingToDefaultValueSwap<TriangleCulling>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->triangleCulling(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingTriangleCullingWithDefaultValueBoth)
{
    auto stateName = States::PROPERTY_TRIANGLE_CULLING;
    auto effectFile = "effect/state/binding/with-default-value/triangle-culling/StatesBindingTriangleCullingWithDefaultValueBoth.effect";
    auto stateMaterialValue = TriangleCulling::NONE;

    testStateBindingToDefaultValueSwap<TriangleCulling>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->triangleCulling(); }
    );
}

/* Stencil function */

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueAlways)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueAlways.effect";
    auto stateMaterialValue = CompareMode::EQUAL;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueGreater)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueGreater.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueGreaterEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueGreaterEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueLess)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueLess.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueLessEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueLessEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueNever)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueNever.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFunctionWithDefaultValueNotEqual)
{
    auto stateName = States::PROPERTY_STENCIL_FUNCTION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-function/StatesBindingStencilFunctionWithDefaultValueNotEqual.effect";
    auto stateMaterialValue = CompareMode::ALWAYS;

    testStateBindingToDefaultValueSwap<CompareMode>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFunction(); }
    );
}

/* Stencil reference */

TEST_F(DrawCallPoolTest, StatesBindingStencilReferenceWithDefaultValue0)
{
    auto stateName = States::PROPERTY_STENCIL_REFERENCE;
    auto effectFile = "effect/state/binding/with-default-value/stencil-reference/StatesBindingStencilReferenceWithDefaultValue0.effect";
    auto stateMaterialValue = 1;

    testStateBindingToDefaultValueSwap<int>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilReference(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilReferenceWithDefaultValue1)
{
    auto stateName = States::PROPERTY_STENCIL_REFERENCE;
    auto effectFile = "effect/state/binding/with-default-value/stencil-reference/StatesBindingStencilReferenceWithDefaultValue1.effect";
    auto stateMaterialValue = 0;

    testStateBindingToDefaultValueSwap<int>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilReference(); }
    );
}

/* Stencil mask */

TEST_F(DrawCallPoolTest, StatesBindingStencilMaskWithDefaultValue0)
{
    auto stateName = States::PROPERTY_STENCIL_MASK;
    auto effectFile = "effect/state/binding/with-default-value/stencil-mask/StatesBindingStencilMaskWithDefaultValue0.effect";
    auto stateMaterialValue = 1;

    testStateBindingToDefaultValueSwap<uint>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilMask(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilMaskWithDefaultValue1)
{
    auto stateName = States::PROPERTY_STENCIL_MASK;
    auto effectFile = "effect/state/binding/with-default-value/stencil-mask/StatesBindingStencilMaskWithDefaultValue1.effect";
    auto stateMaterialValue = 0;

    testStateBindingToDefaultValueSwap<uint>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilMask(); }
    );
}

/* Stencil fail operation */

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueKeep)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueKeep.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueZero.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueReplace)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueReplace.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueIncr)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueIncr.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueIncrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueIncrWrap.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueDecr)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueDecr.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueDecrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueDecrWrap.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilFailOperationWithDefaultValueInvert)
{
    auto stateName = States::PROPERTY_STENCIL_FAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-fail-operation/StatesBindingStencilFailOperationWithDefaultValueInvert.effect";
    auto stateMaterialValue = StencilOperation::DECR;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilFailOperation(); }
    );
}

/* Stencil Z fail operation */

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueKeep)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueKeep.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueZero.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueReplace)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueReplace.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueIncr)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueIncr.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueIncrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueIncrWrap.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueDecr)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueDecr.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueDecrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueDecrWrap.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZFailOperationWithDefaultValueInvert)
{
    auto stateName = States::PROPERTY_STENCIL_ZFAIL_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-fail-operation/StatesBindingStencilZFailOperationWithDefaultValueInvert.effect";
    auto stateMaterialValue = StencilOperation::DECR;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZFailOperation(); }
    );
}

/* Stencil Z pass operation */

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueKeep)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueKeep.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueZero)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueZero.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueReplace)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueReplace.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueIncr)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueIncr.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueIncrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueIncrWrap.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueDecr)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueDecr.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueDecrWrap)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueDecrWrap.effect";
    auto stateMaterialValue = StencilOperation::INVERT;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingStencilZPassOperationWithDefaultValueInvert)
{
    auto stateName = States::PROPERTY_STENCIL_ZPASS_OPERATION;
    auto effectFile = "effect/state/binding/with-default-value/stencil-z-pass-operation/StatesBindingStencilZPassOperationWithDefaultValueInvert.effect";
    auto stateMaterialValue = StencilOperation::DECR;

    testStateBindingToDefaultValueSwap<StencilOperation>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->stencilZPassOperation(); }
    );
}

/* Scissor test */

TEST_F(DrawCallPoolTest, StatesBindingScissorTestWithDefaultValueTrue)
{
    auto stateName = States::PROPERTY_SCISSOR_TEST;
    auto effectFile = "effect/state/binding/with-default-value/scissor-test/StatesBindingScissorTestWithDefaultValueTrue.effect";
    auto stateMaterialValue = false;

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->scissorTest(); }
    );
}

TEST_F(DrawCallPoolTest, StatesBindingScissorTestWithDefaultValueFalse)
{
    auto stateName = States::PROPERTY_SCISSOR_TEST;
    auto effectFile = "effect/state/binding/with-default-value/scissor-test/StatesBindingScissorTestWithDefaultValueFalse.effect";
    auto stateMaterialValue = true;

    testStateBindingToDefaultValueSwap<bool>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->scissorTest(); }
    );
}

/* Scissor box */

TEST_F(DrawCallPoolTest, StatesBindingScissorBoxWithDefaultValueArray)
{
    auto stateName = States::PROPERTY_SCISSOR_BOX;
    auto effectFile = "effect/state/binding/with-default-value/scissor-box/StatesBindingScissorBoxWithDefaultValueArray.effect";
    auto stateMaterialValue = math::ivec4(0);

    testStateBindingToDefaultValueSwap<math::ivec4>(
        stateMaterialValue,
        stateName,
        effectFile,
        [](DrawCall* d) { return d->scissorBox(); }
    );
}

/* Target */

TEST_F(DrawCallPoolTest, StatesBindingTargetWithDefaultValueSize)
{
    auto stateName = States::PROPERTY_TARGET;
    auto effectFile = "effect/state/binding/with-default-value/target/StatesBindingTargetWithDefaultValueSize.effect";
    auto resourceId = 0;
    auto stateMaterialValue = TextureSampler("TEST", &resourceId);
    auto renderTargetName = "test-render-target";
    auto renderTargetSize = math::ivec2(1024, 1024);

    testStateTargetBindingToDefaultValueSwap(
        effectFile,
        stateName,
        stateMaterialValue,
        renderTargetName,
        renderTargetSize
    );
}

TEST_F(DrawCallPoolTest, StatesBindingTargetWithDefaultValueWidthHeight)
{
    auto stateName = States::PROPERTY_TARGET;
    auto effectFile = "effect/state/binding/with-default-value/target/StatesBindingTargetWithDefaultValueWidthHeight.effect";
    auto resourceId = 0;
    auto stateMaterialValue = TextureSampler("TEST", &resourceId);
    auto renderTargetName = "test-render-target";
    auto renderTargetSize = math::ivec2(2048, 1024);

    testStateTargetBindingToDefaultValueSwap(
        effectFile,
        stateName,
        stateMaterialValue,
        renderTargetName,
        renderTargetSize
    );
}
