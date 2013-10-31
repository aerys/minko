/*
Copyright (c) 2013 Aerys

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

#include "OculusVRCamera.hpp"

#include "minko/scene/Node.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/component/OculusPerspectiveCamera.hpp"
#include "minko/render/Effect.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

OculusVRCamera::OculusVRCamera(float aspectRatio) :
	_aspectRatio(aspectRatio)
{

}

void
OculusVRCamera::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&OculusVRCamera::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&OculusVRCamera::targetRemovedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
OculusVRCamera::targetAddedHandler(AbsCmpPtr component, NodePtr target)
{
	if (targets().size() > 1)
		throw std::logic_error("The OculusVRCamera component cannot have more than 1 target.");

	auto sceneManager = target->root()->component<SceneManager>();

	if (!sceneManager)
		throw std::logic_error("Unable to find the SceneManager.");

	auto context = sceneManager->assets()->context();
	const uint targetSize = 2048;
	const float worldFactor = 1.f;

	_hmd = {
		1280.f,
		800.f,
		0.14976f,
		0.0936f,
		0.064f,
		0.0635f,
		0.041f,
		Vector4::create(1.0, 0.22, 0.24, 0.0)
	};

	auto aspect = _hmd.hResolution / (2.f * _hmd.vResolution);
	auto lensShift = 1.f - 2.f * _hmd.lensSeparationDistance / _hmd.hScreenSize;
	auto s = -1.f - lensShift;
	auto distScale = _hmd.distortionK->x() + _hmd.distortionK->y() * powf(s, 2) + _hmd.distortionK->z() * powf(s, 4)
		+ _hmd.distortionK->w() * powf(s, 6);
	auto fov = 2. * atanf((distScale * _hmd.vScreenSize / 2.f) / _hmd.eyeToScreenDistance);

	// left eye
	auto leftEye = scene::Node::create();
	auto leftEyeTexture = render::Texture::create(context, targetSize, targetSize, false, true);
	auto leftRenderer = Renderer::create();
	
	_leftCamera = PerspectiveCamera::create(1.f, fov);

	leftEyeTexture->upload();
	leftRenderer->target(leftEyeTexture);
	leftEye->addComponent(leftRenderer);
	leftEye->addComponent(_leftCamera);
	leftEye->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(-_hmd.interpupillaryDistance * worldFactor * .5f)));

	// right eye
	auto rightEye = scene::Node::create();
	auto rightEyeTexture = render::Texture::create(context, targetSize, targetSize, false, true);
	auto rightRenderer = Renderer::create();

	_rightCamera = PerspectiveCamera::create(1.f, fov);

	rightEyeTexture->upload();
	rightRenderer->target(rightEyeTexture);
	rightEye->addComponent(rightRenderer);
	rightEye->addComponent(_rightCamera);
	rightEye->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(_hmd.interpupillaryDistance * worldFactor * .5f)));

	_root = scene::Node::create("oculus vr");
	_root->addChild(leftEye)->addChild(rightEye);
	target->addChild(_root);

	// post processing effect
	_renderer = Renderer::create();

	auto ppFx = sceneManager->assets()->effect("effect/OculusVR/OculusVR.effect");

	if (!ppFx)
		throw std::logic_error("OculusVR.effect has not been loaded.");

	auto ppScene = scene::Node::create()
		->addComponent(_renderer)
		->addComponent(Surface::create(
			geometry::QuadGeometry::create(sceneManager->assets()->context()),
			data::StructureProvider::create("oculusvr")
				->set("leftEyeTexture",		leftEyeTexture)
				->set("leftLensCenter",		Vector2::create(.25f + lensShift * .5f, .5f))
				->set("rightEyeTexture",	rightEyeTexture)
				->set("rightLensCenter",	Vector2::create(.75f - lensShift * .5f, .5f)),
			ppFx
		));

	ppFx->setUniform("uScaleIn", 4.f, 2.f);
	ppFx->setUniform("uScale", .25f, .5f);
	ppFx->setUniform("uHmdWarpParam", _hmd.distortionK->x(), _hmd.distortionK->y(), _hmd.distortionK->z(), _hmd.distortionK->w());

	_renderEndSlot = sceneManager->renderingEnd()->connect(std::bind(
		&OculusVRCamera::renderEndHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
OculusVRCamera::targetRemovedHandler(AbsCmpPtr component, NodePtr target)
{
	target->removeChild(_root);
	_renderEndSlot = nullptr;
}

void
OculusVRCamera::renderEndHandler(std::shared_ptr<SceneManager>		sceneManager,
								 uint								frameId,
								 std::shared_ptr<render::Texture>	renderTarget)
{
	_renderer->render(sceneManager->assets()->context());
}
