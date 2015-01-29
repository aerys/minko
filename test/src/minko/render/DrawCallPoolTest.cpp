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

using namespace minko;
using namespace minko::render;

TEST_F(DrawCallPoolTest, Constructor)
{
    DrawCallPool drawCallPool;
}

TEST_F(DrawCallPoolTest, UniformDefaultToBindingSwap)
{
    auto fx = MinkoTests::loadEffect("effect/OneUniformBindingAndDefault.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    std::unordered_map<std::string, std::string> variables = { { "geometryUuid", geom->uuid() } };

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ((*drawCalls.first)->boundFloatUniforms().size(), 1);
    ASSERT_EQ(
        (*drawCalls.first)->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );

    auto p = data::Provider::create();

    p->set("diffuseColor", math::vec4(1.));
    targetData.addProvider(p);
    pool.update();

    ASSERT_EQ((*drawCalls.first)->boundFloatUniforms().size(), 1);
    ASSERT_NE(
        (*drawCalls.first)->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
    ASSERT_EQ(
        (*drawCalls.first)->boundFloatUniforms()[0].data,
        math::value_ptr(targetData.get<math::vec4>("diffuseColor"))
    );
}

TEST_F(DrawCallPoolTest, UniformBindingToDefaultSwap)
{
    auto fx = MinkoTests::loadEffect("effect/OneUniformBindingAndDefault.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    std::unordered_map<std::string, std::string> variables = { { "geometryUuid", geom->uuid() } };

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    auto p = data::Provider::create();

    p->set("diffuseColor", math::vec4(1.));
    targetData.addProvider(p);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ((*drawCalls.first)->boundFloatUniforms().size(), 1);
    ASSERT_NE(
        (*drawCalls.first)->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
    ASSERT_EQ(
        (*drawCalls.first)->boundFloatUniforms()[0].data,
        math::value_ptr(targetData.get<math::vec4>("diffuseColor"))
    );

    p->unset("diffuseColor");
    pool.update();

    ASSERT_EQ((*drawCalls.first)->boundFloatUniforms().size(), 1);
    ASSERT_EQ(
        (*drawCalls.first)->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
}

TEST_F(DrawCallPoolTest, WatchAndDefineIntMacro)
{
    auto fx = MinkoTests::loadEffect("effect/OneIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    std::unordered_map<std::string, std::string> variables;

    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().bindings.size(), 1);
    ASSERT_EQ(targetData.propertyChanged("bar").numCallbacks(), 0);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("bar").numCallbacks(), 1);
    ASSERT_FALSE((*drawCalls.first)->program()->definedMacroNames().find("FOO") != (*drawCalls.first)->program()->definedMacroNames().end());

    auto p = data::Provider::create();
    p->set("bar", 42);
    targetData.addProvider(p);
    pool.update();

    ASSERT_TRUE((*drawCalls.first)->program()->definedMacroNames().find("FOO") != (*drawCalls.first)->program()->definedMacroNames().end());

    p->unset("bar");
    pool.update();

    ASSERT_FALSE((*drawCalls.first)->program()->definedMacroNames().find("FOO") != (*drawCalls.first)->program()->definedMacroNames().end());
}

TEST_F(DrawCallPoolTest, WatchAndDefineVariableIntMacro)
{
    auto fx = MinkoTests::loadEffect("effect/OneVariableIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto p = data::Provider::create();
    std::string materialUuid = p->uuid();
    std::unordered_map<std::string, std::string> variables = {{ "materialUuid", materialUuid }};

    ASSERT_EQ(fx->techniques().at("default")[0]->macroBindings().bindings.size(), 1);
    ASSERT_EQ(targetData.propertyChanged("bar").numCallbacks(), 0);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid + "].bar").numCallbacks(), 1);
    ASSERT_FALSE((*drawCalls.first)->program()->definedMacroNames().find("FOO") != (*drawCalls.first)->program()->definedMacroNames().end());

    p->set("bar", 42);
    targetData.addProvider(p, "material");
    pool.update();

    ASSERT_TRUE((*drawCalls.first)->program()->definedMacroNames().find("FOO") != (*drawCalls.first)->program()->definedMacroNames().end());

    p->unset("bar");
    pool.update();

    ASSERT_FALSE((*drawCalls.first)->program()->definedMacroNames().find("FOO") != (*drawCalls.first)->program()->definedMacroNames().end());
}

TEST_F(DrawCallPoolTest, StopWatchingMacroAfterDrawCallsRemoved)
{
    auto fx = MinkoTests::loadEffect("effect/OneVariableIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto p = data::Provider::create();
    std::string materialUuid = p->uuid();
    std::unordered_map<std::string, std::string> variables = {{ "materialUuid", materialUuid }};

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid + "].bar").numCallbacks(), 1);

    pool.removeDrawCalls(drawCalls);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid + "].bar").numCallbacks(), 0);
}

/** Sampler states binding swap **/

TEST_F(DrawCallPoolTest, SamplerStateSwapWrapModeBindingToDefaultClamp)
{
    auto samplerStateMaterialValue = WrapMode::REPEAT;
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingWrapModeWithDefaultValueClamp.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, material->data()->get<WrapMode>(samplerStateBindingName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, pass->uniformBindings().defaultValues.get<WrapMode>(sampleStateUniformName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapWrapModeBindingToDefaultRepeat)
{
    auto samplerStateMaterialValue = WrapMode::CLAMP;
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingWrapModeWithDefaultValueClamp.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, material->data()->get<WrapMode>(samplerStateBindingName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, pass->uniformBindings().defaultValues.get<WrapMode>(sampleStateUniformName));
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapTextureFilterBindingToDefaultLinear)
{
    auto samplerStateMaterialValue = TextureFilter::NEAREST;
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingTextureFilterWithDefaultValueLinear.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, material->data()->get<TextureFilter>(samplerStateBindingName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);

    auto value = pass->uniformBindings().defaultValues.get<TextureFilter>(sampleStateUniformName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, pass->uniformBindings().defaultValues.get<TextureFilter>(sampleStateUniformName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapTextureFilterBindingToDefaultNearest)
{
    auto samplerStateMaterialValue = TextureFilter::LINEAR;
    auto samplerStateProperty = SamplerStates::PROPERTY_TEXTURE_FILTER;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingTextureFilterWithDefaultValueNearest.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    auto p = data::Provider::create();
    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    auto value = material->data()->get<TextureFilter>(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, material->data()->get<TextureFilter>(samplerStateBindingName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);

    material->data()->unset(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, pass->uniformBindings().defaultValues.get<TextureFilter>(sampleStateUniformName));
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SamplerStateSwapMipFilterBindingToDefaultNone)
{
    auto samplerStateMaterialValue = MipFilter::LINEAR;
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingMipFilterWithDefaultValueNone.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, material->data()->get<MipFilter>(samplerStateBindingName));

    material->data()->unset(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, pass->uniformBindings().defaultValues.get<MipFilter>(sampleStateUniformName));
}

TEST_F(DrawCallPoolTest, SamplerStateSwapMipFilterBindingToDefaultLinear)
{
    auto samplerStateMaterialValue = MipFilter::NONE;
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingMipFilterWithDefaultValueLinear.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, material->data()->get<MipFilter>(samplerStateBindingName));

    material->data()->unset(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, pass->uniformBindings().defaultValues.get<MipFilter>(sampleStateUniformName));
}

TEST_F(DrawCallPoolTest, SamplerStateSwapMipFilterBindingToDefaultNearest)
{
    auto samplerStateMaterialValue = MipFilter::NONE;
    auto samplerStateProperty = SamplerStates::PROPERTY_MIP_FILTER;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingMipFilterWithDefaultValueNearest.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());
    material->data()->set(samplerStateBindingName, samplerStateMaterialValue);

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, material->data()->get<MipFilter>(samplerStateBindingName));

    material->data()->unset(samplerStateBindingName);

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, pass->uniformBindings().defaultValues.get<MipFilter>(sampleStateUniformName));
}

/** Sampler states binding with no binding value and no default value **/

TEST_F(DrawCallPoolTest, SamplerStatesBindingWrapModeNoDefaultValue)
{
    auto samplerStateProperty = SamplerStates::PROPERTY_WRAP_MODE;

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingWrapModeNoDefaultValue.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
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

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingTextureFilterNoDefaultValue.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
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

    auto fx = MinkoTests::loadEffect("effect/SamplerStatesBindingMipFilterNoDefaultValue.effect");
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
    std::unordered_map<std::string, std::string> variables = { { "materialUuid", material->uuid() } };

    material->data()->set(samplerBindingName, texture->sampler());

    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    variables.insert({ "geometryUuid", geom->uuid() });

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

    auto& samplers = (*drawCalls.first)->samplers();
    auto& sampler = samplers[0];

    ASSERT_EQ(samplers.size(), 1);
    ASSERT_FALSE(pass->uniformBindings().defaultValues.hasProperty(sampleStateUniformName));

    ASSERT_EQ(*sampler.wrapMode, SamplerStates::DEFAULT_WRAP_MODE);
    ASSERT_EQ(*sampler.textureFilter, SamplerStates::DEFAULT_TEXTURE_FILTER);
    ASSERT_EQ(*sampler.mipFilter, SamplerStates::DEFAULT_MIP_FILTER);
}

TEST_F(DrawCallPoolTest, SameMacroBindingDifferentVariables)
{
    auto fx = MinkoTests::loadEffect("effect/OneVariableIntMacroBinding.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto p1 = data::Provider::create();
    auto p2 = data::Provider::create();
    std::string materialUuid1 = p1->uuid();
    std::string materialUuid2 = p2->uuid();
    std::unordered_map<std::string, std::string> variables1 = { { "materialUuid", materialUuid1 } };
    std::unordered_map<std::string, std::string> variables2 = { { "materialUuid", materialUuid2 } };

    targetData.addProvider(p1, component::Surface::MATERIAL_COLLECTION_NAME);
    targetData.addProvider(p2, component::Surface::MATERIAL_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, "default", variables1, rootData, rendererData, targetData);
    auto drawCalls2 = pool.addDrawCalls(fx, "default", variables2, rootData, rendererData, targetData);

    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid1 + "].bar").numCallbacks(), 1);
    ASSERT_EQ(targetData.propertyChanged("material[" + materialUuid2 + "].bar").numCallbacks(), 1);
}
