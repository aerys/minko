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

#include "minko/component/DirectionalLight.hpp"

#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::component;

DirectionalLight::DirectionalLight(float diffuse, float specular) :
	AbstractDiscreteLight("directionalLight", diffuse, specular),
	_shadowMappingEnabled(true),
	_projection(math::ortho(-5.f, 5.f, -5.f, 5.f, -10.f, 10.f))
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
	auto size = 256;

	if (!fx)
	{
		auto texture = render::Texture::create(assets->context(), size, size, false, true);
		texture->upload();
		assets->texture("vsm-depth-texture", texture);

		auto loader = file::Loader::create(assets->loader());
		// FIXME: support async loading of the ShadowMapping.effect file
		loader->options()->loadAsynchronously(false);
		loader->queue(effectName);
		loader->load();
		fx = assets->effect(effectName);
	}

	_shadowMap = render::Texture::create(assets->context(), size, size, false, true);
	_shadowMap->upload();
	data()
		->set("shadowMap", _shadowMap->sampler())
		->set("shadowMapSize", static_cast<float>(size))
		->set("shadowBias", 0.01f)
		->set("shadowSpread", 1.2f);

	_renderer = component::Renderer::create(0xffffffff, _shadowMap, fx, render::Priority::FIRST);
	_renderer->effectVariables()["lightUuid"] = data()->uuid();
	_renderer->layoutMask(256);
	target()->addComponent(_renderer);
}

void
DirectionalLight::computeShadowProjection(const math::mat4& viewProjection)
{
    math::mat4 t = _view * math::inverse(viewProjection);
    std::vector<math::vec4> v = {
        t * math::vec4(-1.f, 1.f, 0.f, 1.f),
        t * math::vec4(1.f, 1.f, 0.f, 1.f),
        t * math::vec4(1.f, -1.f, 0.f, 1.f),
        t * math::vec4(-1.f, -1.f, 0.f, 1.f),
        t * math::vec4(-1.f, 1.f, 1.f, 1.f),
        t * math::vec4(1.f, 1.f, 1.f, 1.f),
        t * math::vec4(1.f, -1.f, 1.f, 1.f),
        t * math::vec4(-1.f, -1.f, 1.f, 1.f)
    };

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

    for (auto& p : v)
    {
        p = p / p.w;

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

    _projection = math::ortho<float>(
        bottomLeft.x, topRight.x,
        bottomLeft.y, topRight.y,
        std::min(-1.f, -topRight.z), -bottomLeft.z
    );

    updateWorldToScreenMatrix();
}

void
DirectionalLight::updateWorldToScreenMatrix()
{
    auto farMinusNear = 2.f / _projection[2][2];
    auto farPlusNear = _projection[3][2] * farMinusNear;
    auto near = (farMinusNear + farPlusNear) / 2.f;
    auto far = farPlusNear - near;

	if (target() && target()->data().hasProperty("modelToWorldMatrix"))
    	_view = math::inverse(target()->data().get<minko::math::mat4>("modelToWorldMatrix"));
	else
		_view = math::mat4(1.f);

    data()
        ->set("zNear", near)
        ->set("zFar", far)
        ->set("viewProjection", _projection * _view);
}

void
DirectionalLight::updateRoot(std::shared_ptr<scene::Node> root)
{
	AbstractRootDataComponent::updateRoot(root);

	if (root && _shadowMappingEnabled && !_renderer && root->hasComponent<SceneManager>())
		initializeShadowMapping(root->component<SceneManager>()->assets());
}

void
DirectionalLight::targetRemoved(minko::scene::Node::Ptr target)
{
	AbstractDiscreteLight::targetAdded(target);

	if (_renderer && target->hasComponent(_renderer))
    	target->removeComponent(_renderer);
}
