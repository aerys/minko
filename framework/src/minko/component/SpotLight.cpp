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

#include "minko/component/SpotLight.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/ShadowMappingTechnique.hpp"
#include "minko/component/Transform.hpp"
#include "minko/render/Texture.hpp"
#include "minko/scene/Layout.hpp"

using namespace minko;
using namespace minko::component;

const uint SpotLight::MIN_SHADOWMAP_SIZE = 32;
const uint SpotLight::MAX_SHADOWMAP_SIZE = 1024;
const uint SpotLight::DEFAULT_SHADOWMAP_SIZE = 512;

SpotLight::SpotLight(float diffuse,
					 float specular,
                     float innerAngleRadians,
                     float outerAngleRadians,
                     float attenuationConstant,
                     float attenuationLinear,
                     float attenuationQuadratic) :
	AbstractDiscreteLight("spotLight", diffuse, specular),
    _shadowMappingEnabled(false),
    _shadowMap(nullptr),
    _shadowMapSize(0),
    _shadowRenderer()
{
    updateModelToWorldMatrix(math::mat4(1.f));

    attenuationCoefficients(math::vec3(attenuationConstant, attenuationLinear, attenuationQuadratic));
	innerConeAngle(innerAngleRadians);
	outerConeAngle(outerAngleRadians);
}

SpotLight::SpotLight(const SpotLight& spotlight, const CloneOption& option) :
	AbstractDiscreteLight("spotLight", spotlight.diffuse(), spotlight.specular())
{
    updateModelToWorldMatrix(math::mat4(1.f));

    auto test = spotlight.attenuationCoefficients();

	data()->set("attenuationCoeffs", spotlight.attenuationCoefficients());
	data()->set("cosInnerConeAngle", spotlight.innerConeAngle());
	data()->set("cosOuterConeAngle", spotlight.outerConeAngle());
}

AbstractComponent::Ptr
SpotLight::clone(const CloneOption& option)
{
	auto light = std::shared_ptr<SpotLight>(new SpotLight(*this, option));

	return light;
}

void
SpotLight::updateModelToWorldMatrix(const math::mat4& modelToWorld)
{
	data()
		->set("position",	(modelToWorld * math::vec4(0.f, 0.f, 0.f, 1.f)).xyz())
		->set("direction",	math::normalize(math::mat3(modelToWorld) * math::vec3(0.f, 0.f, -1.f)));
}

float
SpotLight::innerConeAngle() const
{
    return acos(data()->get<float>("cosInnerConeAngle"));
}

SpotLight&
SpotLight::innerConeAngle(float radians)
{
	data()->set<float>(
		"cosInnerConeAngle",
		cosf(std::max(0.0f, std::min(0.5f * math::pi<float>(), radians)))
	);

	return *this;
}

float
SpotLight::outerConeAngle() const
{
    return acos(data()->get<float>("cosOuterConeAngle"));
}

SpotLight&
SpotLight::outerConeAngle(float radians)
{
	data()->set<float>(
		"cosOuterConeAngle",
        cosf(std::max(0.0f, std::min(0.5f * math::pi<float>(), radians)))
	);

	return *this;
}

const math::vec3&
SpotLight::attenuationCoefficients() const
{
	return data()->get<math::vec3>("attenuationCoeffs");
}

SpotLight&
SpotLight::attenuationCoefficients(float constant, float linear, float quadratic)
{
	return attenuationCoefficients(math::vec3(constant, linear, quadratic));
}

SpotLight&
SpotLight::attenuationCoefficients(const math::vec3& value)
{
	data()->set("attenuationCoeffs", value);

	return *this;
}

bool
SpotLight::attenuationEnabled() const
{
	auto& coef = attenuationCoefficients();

	return !(coef.x < 0.0f || coef.y < 0.0f || coef.z < 0.0f);
}

void
SpotLight::updateRoot(std::shared_ptr<scene::Node> root)
{
    AbstractRootDataComponent::updateRoot(root);

    if (root && _shadowMappingEnabled && !_shadowMap)
        initializeShadowMapping();
}

bool
SpotLight::initializeShadowMapping()
{
    if (!target() || !target()->root()->hasComponent<SceneManager>())
        return false;

    auto assets = target()->root()->component<SceneManager>()->assets();
    auto effectName = "effect/SpotLightShadowMap.effect";
    auto fx = assets->effect(effectName);

    auto smTechnique = target()->root()->hasComponent<ShadowMappingTechnique>()
        ? target()->root()->data()
        .get<ShadowMappingTechnique::Technique>("shadowMappingTechnique")
        : ShadowMappingTechnique::Technique::DEFAULT;

    if (!fx)
    {
        auto texture = assets->texture("shadow-map-tmp");

        if (!texture)
        {
            // This texture is used only for ESM, but loading SpotLightShadowMap.effect will throw if the asset does not exist.
            // Thus, we create a dummy texture that we simply don't upload on the GPU.
            texture = render::Texture::create(assets->context(), _shadowMapSize, _shadowMapSize, false, true);
            if (smTechnique == ShadowMappingTechnique::Technique::ESM)
                texture->upload();
            assets->texture("shadow-map-tmp", texture);
        }

        texture = assets->texture("shadow-map-tmp-2");
        if (!texture)
        {
            texture = render::Texture::create(assets->context(), _shadowMapSize, _shadowMapSize, false, true);
            if (smTechnique == ShadowMappingTechnique::Technique::ESM)
                texture->upload();
            assets->texture("shadow-map-tmp-2", texture);
        }

        auto loader = file::Loader::create(assets->loader());
        // FIXME: support async loading of the ShadowMapping.effect file
        loader->options()->loadAsynchronously(false);
        loader->queue(effectName);
        loader->load();
        fx = assets->effect(effectName);
    }

    _shadowMap = render::Texture::create(assets->context(), _shadowMapSize, _shadowMapSize, false, true);
    _shadowMap->upload();

    data()
        ->set("shadowMap", _shadowMap->sampler())
        ->set("shadowMaxDistance", 0.9f)
        ->set("shadowSpread", 1.f)
        ->set("shadowBias", -0.001f)
        ->set("shadowMapSize", static_cast<float>(_shadowMapSize));

    auto techniqueName = std::string("shadow-map-cascade");
    if (smTechnique == ShadowMappingTechnique::Technique::ESM)
        techniqueName += "-esm";

    auto renderer = component::Renderer::create(
        0xffffffff,
        _shadowMap,
        fx,
        techniqueName,
        render::Priority::FIRST
    );

    renderer->clearBeforeRender(true);
    renderer->effectVariables().push_back({ "lightUuid", data()->uuid() });
    // renderer->effectVariables()["shadowProjectionId"] = std::to_string(i);
    renderer->layoutMask(scene::BuiltinLayout::CAST_SHADOW);
    target()->addComponent(renderer);

    _shadowRenderer = renderer;

    computeShadowProjection();

    // Create specific shadow projection
    auto zNear = 1.f;
    auto zFar = 40.f;
    auto fov = outerConeAngle() * 2.f;
    auto aspectRatio = 1.f;

    _shadowProjection = math::perspective(fov, aspectRatio, zNear, zFar);

    data()
        ->set("zNear", zNear)
        ->set("zFar", zFar);

    computeShadowProjection();

    return true;
}

void
SpotLight::computeShadowProjection()
{
    if (target() && target()->data().hasProperty("modelToWorldMatrix"))
        _view = math::inverse(target()->data().get<minko::math::mat4>("modelToWorldMatrix"));
    else
        _view = math::mat4(1.f);

    data()->set("viewProjection", _shadowProjection * _view);
}

void
SpotLight::enableShadowMapping(uint shadowMapSize)
{
    if (!_shadowMappingEnabled || shadowMapSize != _shadowMapSize)
    {
        if (!_shadowMap || shadowMapSize != _shadowMapSize)
        {
            // FIXME: do not completely re-init shadow mapping when just the shadow map size changes
            _shadowMapSize = shadowMapSize;
            initializeShadowMapping();
        }
        else
        {
            if (_shadowRenderer)
                _shadowRenderer->enabled(true);

            data()->set("shadowMap", _shadowMap->sampler());
        }

        _shadowMappingEnabled = true;
    }
}

void
SpotLight::disableShadowMapping(bool disposeResources)
{
    if (_shadowMappingEnabled)
    {
        if (_shadowRenderer)
            _shadowRenderer->enabled(false);

        data()->unset("shadowMap");

        if (disposeResources)
        {
            _shadowMap = nullptr;

            if (_shadowRenderer && target()->hasComponent(_shadowRenderer))
            {
                target()->removeComponent(_shadowRenderer);
                // renderer = nullptr;
            }
        }

        _shadowMappingEnabled = false;
    }
}
