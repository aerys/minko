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
#include "minko/scene/NodeSet.hpp"
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
#include "minko/render/Effect.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

OculusVRCamera::OculusVRCamera(float aspectRatio) :
	_aspectRatio(aspectRatio),
	_sceneManager(nullptr),
	_root(nullptr),
	_leftCamera(nullptr),
	_rightCamera(nullptr),
	_renderer(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_renderEndSlot(nullptr)
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

	_addedSlot = target->added()->connect(std::bind(
		&OculusVRCamera::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&OculusVRCamera::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	addedHandler(target->root(), target, target->parent());
}


void
OculusVRCamera::targetRemovedHandler(AbsCmpPtr component, NodePtr target)
{
	_addedSlot		= nullptr;
	_removedSlot	= nullptr;

	removedHandler(target->root(), target, target->parent());
}

void
OculusVRCamera::addedHandler(NodePtr node, NodePtr target, NodePtr parent)
{
	findSceneManager();
}

void
OculusVRCamera::removedHandler(NodePtr node, NodePtr target, NodePtr parent)
{
	findSceneManager();
}

void
OculusVRCamera::findSceneManager()
{
	NodeSet::Ptr roots = NodeSet::create(targets())
		->roots()
		->where([](NodePtr node)
		{
			return node->hasComponent<SceneManager>();
		});

	if (roots->nodes().size() > 1)
		throw std::logic_error("OculusVRCamera cannot be in two separate scenes.");
	else if (roots->nodes().size() == 1)
		setSceneManager(roots->nodes()[0]->component<SceneManager>());		
	else
		setSceneManager(nullptr);
}

void
OculusVRCamera::setSceneManager(SceneManager::Ptr sceneManager)
{
	if (_sceneManager == sceneManager)
		return;

	if (sceneManager == nullptr)
	{
		_sceneManager	= nullptr;
		_renderer		= nullptr;
		_leftCamera		= nullptr;
		_rightCamera	= nullptr;

		if (_root)
			targets().front()->removeChild(_root);

		_root			= nullptr;
		_renderEndSlot	= nullptr;

		return;
	}

	_sceneManager	= sceneManager;

	auto context = _sceneManager->assets()->context();
	const uint targetSize = 2048;
	const float worldFactor = 1.f;

	_hmd.hResolution			= 1280.0f;
	_hmd.vResolution			= 800.0f;
	_hmd.hScreenSize			= 0.14976f;
	_hmd.vScreenSize			= 0.0936f;
	_hmd.interpupillaryDistance	= 0.064f;
	_hmd.lensSeparationDistance	= 0.0635f;
	_hmd.eyeToScreenDistance	= 0.041f;
	_hmd.distortionK			= Vector4::create(1.0f, 0.22f, 0.24f, 0.0f);

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
	leftEye->addComponent(Transform::create(
		Matrix4x4::create()->appendTranslation(-_hmd.interpupillaryDistance * worldFactor * .5f)
	));

	// right eye
	auto rightEye = scene::Node::create();
	auto rightEyeTexture = render::Texture::create(context, targetSize, targetSize, false, true);
	auto rightRenderer = Renderer::create();

	_rightCamera = PerspectiveCamera::create(1.f, fov);

	rightEyeTexture->upload();
	rightRenderer->target(rightEyeTexture);
	rightEye->addComponent(rightRenderer);
	rightEye->addComponent(_rightCamera);
	rightEye->addComponent(Transform::create(
		Matrix4x4::create()->appendTranslation(_hmd.interpupillaryDistance * worldFactor * .5f)
	));

	_root = scene::Node::create("oculus vr");
	_root->addChild(leftEye)->addChild(rightEye);
	targets().front()->addChild(_root);

	// post processing effect
	_renderer = Renderer::create();

	auto ppFx = _sceneManager->assets()->effect("effect/OculusVR/OculusVR.effect");

	if (!ppFx)
		throw std::logic_error("OculusVR.effect has not been loaded.");

	auto ppScene = scene::Node::create()
		->addComponent(_renderer)
		->addComponent(Surface::create(
			geometry::QuadGeometry::create(_sceneManager->assets()->context()),
			data::StructureProvider::create("oculusvr")
				->set("leftEyeTexture",		leftEyeTexture)
				->set("leftLensCenter",		Vector2::create(.25f + lensShift * .5f, .5f))
				->set("rightEyeTexture",	rightEyeTexture)
				->set("rightLensCenter",	Vector2::create(.75f - lensShift * .5f, .5f)),
			ppFx
		));

	ppFx->setUniform("uScaleIn", 4.f, 2.f);
	ppFx->setUniform("uScale", .25f, .5f);
	ppFx->setUniform("uDistortionK01", _hmd.distortionK->x(), _hmd.distortionK->y());
	ppFx->setUniform("uDistortionK23", _hmd.distortionK->z(), _hmd.distortionK->w());
	//ppFx->setUniform("uHmdWarpParam", _hmd.distortionK->x(), _hmd.distortionK->y(), _hmd.distortionK->z(), _hmd.distortionK->w());

	_renderEndSlot = sceneManager->renderingEnd()->connect(std::bind(
		&OculusVRCamera::renderEndHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
OculusVRCamera::renderEndHandler(std::shared_ptr<SceneManager>		sceneManager,
								 uint								frameId,
								 std::shared_ptr<render::Texture>	renderTarget)
{
	_renderer->render(sceneManager->assets()->context());
}
