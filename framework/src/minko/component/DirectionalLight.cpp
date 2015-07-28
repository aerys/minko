/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 	IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/component/DirectionalLight.hpp"

#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::component;

const uint DirectionalLight::DEFAULT_NUM_SHADOW_CASCADES 	= 4;
const uint DirectionalLight::MAX_NUM_SHADOW_CASCADES 		= 4;
const uint DirectionalLight::MIN_SHADOWMAP_SIZE				= 32;
const uint DirectionalLight::MAX_SHADOWMAP_SIZE				= 1024;
const uint DirectionalLight::DEFAULT_SHADOWMAP_SIZE			= 512;

DirectionalLight::DirectionalLight(float diffuse, float specular) :
	AbstractDiscreteLight("directionalLight", diffuse, specular),
	_shadowMappingEnabled(false),
	_numShadowCascades(0),
	_shadowMap(nullptr),
	_shadowMapSize(0),
	_shadowRenderers()
{
    updateModelToWorldMatrix(math::mat4(1.f));
}

DirectionalLight::DirectionalLight(const DirectionalLight& directionalLight, const CloneOption& option) :
	AbstractDiscreteLight("directionalLight", directionalLight.diffuse(), directionalLight.specular())
{
    updateModelToWorldMatrix(math::mat4(1.f));
}

AbstractComponent::Ptr
DirectionalLight::clone(const CloneOption& option)
{
	return std::shared_ptr<DirectionalLight>(new DirectionalLight(*this, option));
}

void
DirectionalLight::updateModelToWorldMatrix(const math::mat4& modelToWorld)
{
	_worldDirection = math::normalize(math::mat3x3(modelToWorld) * math::vec3(0.f, 0.f, -1.f));
	data()->set("direction", _worldDirection);

	updateWorldToScreenMatrix();
}

bool
DirectionalLight::initializeShadowMapping()
{
	if (!target() || !target()->root()->hasComponent<SceneManager>())
		return false;

	auto assets = target()->root()->component<SceneManager>()->assets();
	auto effectName = "effect/ShadowMap.effect";
	auto fx = assets->effect(effectName);

	if (!fx)
	{
		auto texture = assets->texture("shadow-map-tmp");

		if (!texture)
		{
			texture = render::Texture::create(assets->context(), _shadowMapSize, _shadowMapSize, false, true);
			texture->upload();
			assets->texture("shadow-map-tmp", texture);
		}

		auto loader = file::Loader::create(assets->loader());
		// FIXME: support async loading of the ShadowMapping.effect file
		loader->options()->loadAsynchronously(false);
		loader->queue(effectName);
		loader->load();
		fx = assets->effect(effectName);
	}

	_shadowMap = render::Texture::create(assets->context(), _shadowMapSize * 2, _shadowMapSize * 2, false, true);
	_shadowMap->upload();
	data()
		->set("shadowMap", _shadowMap->sampler())
		->set("shadowSpread", 1.f)
		->set("shadowBias", 0.01f)
		->set("shadowMapSize", static_cast<float>(_shadowMapSize));

	std::array<math::ivec4, 4> viewports = {
		math::ivec4(0, _shadowMapSize, _shadowMapSize, _shadowMapSize),
		math::ivec4(_shadowMapSize, _shadowMapSize, _shadowMapSize, _shadowMapSize),
		math::ivec4(0, 0, _shadowMapSize, _shadowMapSize),
		math::ivec4(_shadowMapSize, 0, _shadowMapSize, _shadowMapSize)
	};
	for (auto i = 0u; i < _numShadowCascades; ++i)
	{
		auto renderer = component::Renderer::create(
			0xffffffff,
			_shadowMap,
			fx,
			"shadow-map-cascade" + std::to_string(i),
			render::Priority::FIRST - i
		);

		renderer->clearBeforeRender(i == 0);
		renderer->viewport(viewports[i]);
		renderer->effectVariables().push_back({ "lightUuid", data()->uuid() });
		// renderer->effectVariables()["shadowProjectionId"] = std::to_string(i);
		renderer->layoutMask(256);
		target()->addComponent(renderer);

		_shadowRenderers[i] = renderer;
	}

	computeShadowProjection(math::mat4(1.f), math::perspective(.785f, 1.f, 0.1f, 1000.f));

	return true;
}

std::pair<math::vec3, math::vec3>
DirectionalLight::computeBox(const math::mat4& viewProjection)
{
	math::mat4 t = _view * math::inverse(viewProjection);
    std::vector<math::vec4> v = {
        t * math::vec4(-1.f, 1.f, -1.f, 1.f),
        t * math::vec4(1.f, 1.f, -1.f, 1.f),
        t * math::vec4(1.f, -1.f, -1.f, 1.f),
        t * math::vec4(-1.f, -1.f, -1.f, 1.f),
        t * math::vec4(-1.f, 1.f, 1.f, 1.f),
        t * math::vec4(1.f, 1.f, 1.f, 1.f),
        t * math::vec4(1.f, -1.f, 1.f, 1.f),
        t * math::vec4(-1.f, -1.f, 1.f, 1.f)
    };

	for (auto& p : v)
    	p = p / p.w;

	math::vec3 bottomLeft(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
	);
	math::vec3 topRight(
		-std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max()
	);

	for (const auto& p : v)
	{
		if (p.x < bottomLeft.x)
			bottomLeft.x = p.x;
		if (p.x > topRight.x)
			topRight.x = p.x;
		if (p.y < bottomLeft.y)
			bottomLeft.y = p.y;
		if (p.y > topRight.y)
			topRight.y = p.y;
		if (p.z < bottomLeft.z)
			bottomLeft.z = p.z;
		if (p.z > topRight.z)
			topRight.z = p.z;
	}

	return { bottomLeft, topRight };
}

std::pair<math::vec3, float>
DirectionalLight::computeBoundingSphere(const math::mat4& view, const math::mat4& projection)
{
	math::mat4 invProj = _view * math::inverse(projection * view);

	math::vec4 center = invProj * math::vec4(0.f, 0.f, 0.f, 1.f);
	center /= center.w;

	math::vec4 max = invProj * math::vec4(1.f, 1.f, 1.f, 1.f);
	math::vec4 min = invProj * math::vec4(-1.f, -1.f, -1.f, 1.f);
	max /= max.w;
	min /= min.w;
	float radius = std::max(math::length(math::vec3(max - center)), math::length(math::vec3(min - center)));

	// center = _view * center;

	return std::pair<math::vec3, float>(center.xyz(), radius);
}

void
DirectionalLight::computeShadowProjection(const math::mat4& view,
										  const math::mat4& projection,
										  float 			zFar)
{
    if (!_shadowMappingEnabled)
        return;

	math::mat4 invProjection = math::inverse(projection);
	std::vector<math::vec4> v = {
		invProjection * math::vec4(-1.f, 1.f, -1.f, 1.f),
		invProjection * math::vec4(1.f, 1.f, -1.f, 1.f),
		invProjection * math::vec4(1.f, -1.f, -1.f, 1.f),
		invProjection * math::vec4(-1.f, -1.f, -1.f, 1.f),
		invProjection * math::vec4(-1.f, 1.f, 1.f, 1.f),
		invProjection * math::vec4(1.f, 1.f, 1.f, 1.f),
		invProjection * math::vec4(1.f, -1.f, 1.f, 1.f),
		invProjection * math::vec4(-1.f, -1.f, 1.f, 1.f)
	};

	zFar = std::min(zFar, -(v[4] / v[4].w).z);

	float zNear = -(v[0] / v[0].w).z;
	float fov = atanf(1.f / projection[1][1]) * 2.f;
	float ratio = projection[1][1] / projection[0][0];

	// http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
	// page 7
    auto splitFar = std::vector<float> { zFar, zFar, zFar, zFar };
    auto splitNear = std::vector<float> { zNear, zNear, zNear, zNear };
	float lambda = .8f;
	float j = 1.f;
	for (auto i = 0u; i < _numShadowCascades - 1; ++i, j+= 1.f)
	{
		splitFar[i] = math::mix(
			zNear + (j / (float)_numShadowCascades) * (zFar - zNear),
			zNear * powf(zFar / zNear, j / (float)_numShadowCascades),
			lambda
		);
		splitNear[i + 1] = splitFar[i];
	}

	for (auto i = 0u; i < _numShadowCascades; ++i)
	{
		math::mat4 cameraViewProjection = math::perspective(fov, ratio, zNear, splitFar[i]) * view;
		auto box = computeBox(cameraViewProjection);
		auto projection = math::ortho<float>(
	        box.first.x, box.second.x,
	        box.first.y, box.second.y,
	        -box.second.z, -box.first.z
	    );

		// auto center = (box.second + box.first) / 2.f;
		// auto radius = math::length(box.second - center);
		// auto projection = math::ortho<float>(
		// 	center.x - radius, center.x + radius,
		// 	center.y - radius, center.y + radius,
		// 	-center.z - radius, -center.z + radius
		// );

		// auto projCenter = projection * math::vec4(1.f);// / 2.f;
		// auto q = 1.f / (float)_shadowMapSize;
		// auto rounded = math::round(projCenter / q) * q;
		// auto offset = math::vec3(projCenter.x - rounded.x, projCenter.y - rounded.y, 0.f);
		//
		// projection = math::translate(projection, -offset);

		// _shadowProjections[i] = cameraViewProjection;
		_shadowProjections[i] = projection;

		zNear = splitFar[i];
	}

	for (auto i = _numShadowCascades; i < MAX_NUM_SHADOW_CASCADES; ++i)
	{
		splitFar[i] = -std::numeric_limits<float>::max();
		splitNear[i] = std::numeric_limits<float>::max();
	}

	data()->set("shadowSplitFar", math::make_vec4(&splitFar[0]));
	data()->set("shadowSplitNear", math::make_vec4(&splitNear[0]));

	updateWorldToScreenMatrix();
}

void
DirectionalLight::updateWorldToScreenMatrix()
{
	if (target() && target()->data().hasProperty("modelToWorldMatrix"))
    	_view = math::inverse(target()->data().get<minko::math::mat4>("modelToWorldMatrix"));
	else
		_view = math::mat4(1.f);

    auto zFar = std::vector<float> { 0.f, 0.f, 0.f, 0.f };
    auto zNear = std::vector<float> { 0.f, 0.f, 0.f, 0.f };
    auto viewProjections = std::vector<math::mat4>();

	for (uint i = 0u; i < _numShadowCascades; ++i)
	{
		const math::mat4& projection = _shadowProjections[i];
		auto istr = std::to_string(i);
		auto farMinusNear = 2.f / projection[2][2];
	    auto farPlusNear = projection[3][2] * farMinusNear;

		// auto center = viewProjection * math::vec4(0.f, 0.f, 0.f, 1.f) / 2.f;
		// auto q = 1.f / (float)_shadowMapSize;
		// auto rounded = math::round(center / q) * q;
		// auto offset = math::vec3(center.x - rounded.x, center.y - rounded.y, 0.f);

		// std::cout << math::to_string(offset) << std::endl;
		// viewProjection = math::translate(viewProjection, offset);

		// auto t = viewProjection * math::vec4(3.f, 0.f, 1.f, 1.f) / 2.f;
		// std::cout << math::to_string(t) << std::endl;
		// std::cout << math::to_string(math::round(t / q) * q) << std::endl;

    	// data()
		// 	->set("viewProjection" + istr, 	viewProjection)
		// 	->set("zNear" + istr, 			zNear)
		// 	->set("zFar" + istr, 			zFar);

		zNear[i] = (farMinusNear + farPlusNear) / 2.f;
		zFar[i] = farPlusNear - zNear[i];
		viewProjections.push_back(projection * _view);
	}

	data()
		->set("viewProjection", viewProjections)
		->set("zNear", 			zNear)
		->set("zFar", 			zFar);
}

void
DirectionalLight::updateRoot(std::shared_ptr<scene::Node> root)
{
	AbstractRootDataComponent::updateRoot(root);

	if (root && _shadowMappingEnabled && !_shadowMap)
		initializeShadowMapping();
}

void
DirectionalLight::targetRemoved(minko::scene::Node::Ptr target)
{
	AbstractDiscreteLight::targetRemoved(target);

	for (auto renderer : _shadowRenderers)
		if (renderer && target->hasComponent(renderer))
	    	target->removeComponent(renderer);
}

void
DirectionalLight::enableShadowMapping(uint shadowMapSize, uint numCascades)
{
	if (!_shadowMappingEnabled || shadowMapSize != _shadowMapSize || numCascades != _numShadowCascades)
	{
	    if (!_shadowMap || shadowMapSize != _shadowMapSize || numCascades != _numShadowCascades)
		{
			_numShadowCascades = numCascades;
			// FIXME: do not completely re-init shadow mapping when just the shadow map size changes
			_shadowMapSize = shadowMapSize;
			initializeShadowMapping();
		}
		else
		{
			for (auto renderer : _shadowRenderers)
				if (renderer)
					renderer->enabled(true);

			data()->set("shadowMap", _shadowMap->sampler());
		}

		_shadowMappingEnabled = true;
	}
}

void
DirectionalLight::disableShadowMapping(bool disposeResources)
{
	if (_shadowMappingEnabled)
	{
		for (auto renderer : _shadowRenderers)
			if (renderer)
				renderer->enabled(false);
		data()->unset("shadowMap");

		if (disposeResources)
		{
			_shadowMap = nullptr;

			for (auto& renderer : _shadowRenderers)
				if (renderer && target()->hasComponent(renderer))
				{
			    	target()->removeComponent(renderer);
					// renderer = nullptr;
				}
		}

		_shadowMappingEnabled = false;
	}
}
