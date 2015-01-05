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

const uint DirectionalLight::MAX_NUM_SHADOW_CASCADES = 4;

DirectionalLight::DirectionalLight(float diffuse, float specular) :
	AbstractDiscreteLight("directionalLight", diffuse, specular),
	_shadowMappingEnabled(true),
	_numShadowCascades(3),
	_shadowProjections(3),
	_shadowMaps(3),
	_shadowMapSize(256)
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

void
DirectionalLight::initializeShadowMapping(file::AssetLibrary::Ptr assets)
{
	auto effectName = "effect/ShadowMap.effect";
	auto fx = assets->effect(effectName);

	if (!fx)
	{
		auto texture = render::Texture::create(assets->context(), _shadowMapSize, _shadowMapSize, false, true);
		texture->upload();
		assets->texture("shadow-mapping-tmp", texture);

		auto loader = file::Loader::create(assets->loader());
		// FIXME: support async loading of the ShadowMapping.effect file
		loader->options()->loadAsynchronously(false);
		loader->queue(effectName);
		loader->load();
		fx = assets->effect(effectName);
	}

	_shadowMaps.resize(_numShadowCascades, nullptr);
	for (uint i = 0; i < _numShadowCascades; ++i)
	{
		auto shadowMap = render::Texture::create(assets->context(), _shadowMapSize, _shadowMapSize, false, true);

		shadowMap->upload();
		_shadowMaps[i] = shadowMap;
		data()->set("shadowMap" + std::to_string(i), shadowMap->sampler());
	}
	data()
		->set("shadowSpread", 1.f)
		->set("shadowBias", 0.01f)
		->set("shadowMapSize", static_cast<float>(_shadowMapSize));

	_shadowRenderer = component::Renderer::create(0xffffffff, _shadowMaps[0], fx, render::Priority::FIRST);
	_shadowRenderer->effectVariables()["lightUuid"] = data()->uuid();
	_shadowRenderer->layoutMask(256);
	target()->addComponent(_shadowRenderer);
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
DirectionalLight::computeShadowProjection(const math::mat4& view, const math::mat4& projection)
{
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

	float zNear = -(v[0] / v[0].w).z;
	float zFar = -(v[4] / v[4].w).z;
	float fov = atanf(1.f / projection[1][1]) * 2.f;
	float ratio = projection[1][1] / projection[0][0];

	// http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
	// page 7
	std::vector<float> splits(_numShadowCascades);
	float lambda = .8f;
	float j = 1.f;
	for (auto i = 0; i < _numShadowCascades - 1; ++i, j+= 1.f)
	{
		splits[i] = math::mix(
			zNear + (j / (float)_numShadowCascades) * (zFar - zNear),
			zNear * powf(zFar / zNear, j / (float)_numShadowCascades),
			lambda
		);

	}
	splits[_numShadowCascades - 1] = zFar;

	for (auto i = 0; i < _numShadowCascades; ++i)
	{
		math::mat4 cameraViewProjection = math::perspective(fov, ratio, zNear, splits[i]) * view;
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

		zNear = splits[i];
	}

	data()->set("shadowCascadeDepths", math::make_vec4(&splits[0]));

	updateWorldToScreenMatrix();
}

void
DirectionalLight::updateWorldToScreenMatrix()
{
	if (target() && target()->data().hasProperty("modelToWorldMatrix"))
    	_view = math::inverse(target()->data().get<minko::math::mat4>("modelToWorldMatrix"));
	else
		_view = math::mat4(1.f);

	for (int i = 0; i < _numShadowCascades; ++i)
	{
		const math::mat4& projection = _shadowProjections[i];
		auto istr = std::to_string(i);
		auto farMinusNear = 2.f / projection[2][2];
	    auto farPlusNear = projection[3][2] * farMinusNear;
	    auto zNear = (farMinusNear + farPlusNear) / 2.f;
	    auto zFar = farPlusNear - zNear;
		auto viewProjection = projection * _view;

		// auto center = viewProjection * math::vec4(0.f, 0.f, 0.f, 1.f) / 2.f;
		// auto q = 1.f / (float)_shadowMapSize;
		// auto rounded = math::round(center / q) * q;
		// auto offset = math::vec3(center.x - rounded.x, center.y - rounded.y, 0.f);

		// std::cout << math::to_string(offset) << std::endl;
		// viewProjection = math::translate(viewProjection, offset);

		// auto t = viewProjection * math::vec4(3.f, 0.f, 1.f, 1.f) / 2.f;
		// std::cout << math::to_string(t) << std::endl;
		// std::cout << math::to_string(math::round(t / q) * q) << std::endl;

    	data()
			->set("viewProjection" + istr, 	viewProjection)
			->set("zNear" + istr, 			zNear)
			->set("zFar" + istr, 			zFar);
	}
}

void
DirectionalLight::updateRoot(std::shared_ptr<scene::Node> root)
{
	AbstractRootDataComponent::updateRoot(root);

	if (root && _shadowMappingEnabled && !_shadowRenderer && root->hasComponent<SceneManager>())
		initializeShadowMapping(root->component<SceneManager>()->assets());
}

void
DirectionalLight::targetRemoved(minko::scene::Node::Ptr target)
{
	AbstractDiscreteLight::targetRemoved(target);

	if (_shadowRenderer && target->hasComponent(_shadowRenderer))
    	target->removeComponent(_shadowRenderer);
}
