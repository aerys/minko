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

#include "minko/component/OculusVRCamera.hpp"

#include <OVR.h>

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/AbstractComponent.hpp"
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
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

/*static*/ const float					OculusVRCamera::WORLD_UNIT	= 1.0f;
/*static*/ const unsigned int			OculusVRCamera::TARGET_SIZE	= 1024;

OculusVRCamera::OculusVRCamera(float aspectRatio, float zNear, float zFar) :
	_aspectRatio(aspectRatio),
	_zNear(zNear),
	_zFar(zFar),
	_ovrSystem(new OVR::System()),
	_ovrHMDDevice(nullptr),
	_ovrSensorDevice(nullptr),
	_ovrSensorFusion(nullptr),
	_targetTransform(nullptr),
	_eyePosition(Vector3::create(0.0f, 0.0f, 0.0f)),
	_eyeOrientation(Matrix4x4::create()),
	_sceneManager(nullptr),
	_root(nullptr),
	_leftCamera(nullptr),
	_rightCamera(nullptr),
	_renderer(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_targetComponentAddedHandler(nullptr),
	_targetComponentRemovedHandler(nullptr),
	_renderEndSlot(nullptr)
{

}

OculusVRCamera::~OculusVRCamera()
{
	resetOVRDevice();

	_ovrSystem = nullptr;
}


void
OculusVRCamera::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&OculusVRCamera::targetAddedHandler, 
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()), 
		std::placeholders::_1, 
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&OculusVRCamera::targetRemovedHandler, 
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()), 
		std::placeholders::_1, 
		std::placeholders::_2
	));

	initializeOVRDevice();
}

void
OculusVRCamera::targetAddedHandler(AbsCmpPtr component, NodePtr target)
{
	if (targets().size() > 1)
		throw std::logic_error("The OculusVRCamera component cannot have more than 1 target.");

	_addedSlot = target->added()->connect(std::bind(
		&OculusVRCamera::addedHandler,
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&OculusVRCamera::removedHandler,
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_targetComponentAddedHandler = target->componentAdded()->connect(std::bind(
		&OculusVRCamera::targetComponentAddedHandler,
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_targetComponentRemovedHandler = target->componentRemoved()->connect(std::bind(
		&OculusVRCamera::targetComponentRemovedHandler,
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_targetTransform = target->hasComponent<Transform>() 
		? target->component<Transform>()
		: nullptr;

	addedHandler(target->root(), target, target->parent());

}

void
OculusVRCamera::targetRemovedHandler(AbsCmpPtr component, NodePtr target)
{
	_addedSlot						= nullptr;
	_removedSlot					= nullptr;
	_targetComponentAddedHandler	= nullptr;
	_targetComponentRemovedHandler	= nullptr;

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
OculusVRCamera::targetComponentAddedHandler(Node::Ptr, Node::Ptr, AbstractComponent::Ptr component)
{
	auto transform = std::dynamic_pointer_cast<Transform>(component);

	if (transform)
		_targetTransform = transform;
}

void
OculusVRCamera::targetComponentRemovedHandler(Node::Ptr, Node::Ptr, AbstractComponent::Ptr component)
{
	auto transform = std::dynamic_pointer_cast<Transform>(component);

	if (transform)
		_targetTransform = nullptr;
}

void
OculusVRCamera::resetOVRDevice()
{
	_ovrHMDDevice					= nullptr;
	_ovrSensorDevice				= nullptr;
	_ovrSensorFusion				= nullptr;
}


void
OculusVRCamera::initializeOVRDevice()
{
	resetOVRDevice();

	OVR::Ptr<OVR::DeviceManager>	deviceManager	= *OVR::DeviceManager::Create();
	if (deviceManager == nullptr)
		return;

	_ovrHMDDevice					= std::shared_ptr<OVR::HMDDevice>(deviceManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice());
	if (_ovrHMDDevice == nullptr)
		return;

	_ovrSensorFusion				= std::shared_ptr<OVR::SensorFusion>(new OVR::SensorFusion());
	_ovrSensorDevice				= std::shared_ptr<OVR::SensorDevice>(_ovrHMDDevice->GetSensor());

	if (_ovrSensorFusion == nullptr || _ovrSensorDevice == nullptr)
	{
		_ovrSensorFusion			= nullptr;
		_ovrSensorDevice			= nullptr;
	}

	_ovrSensorFusion->AttachToSensor(_ovrSensorDevice.get());
	_ovrSensorFusion->SetPredictionEnabled(true);
}

void
OculusVRCamera::getHMDInfo(HMDInfo& hmdInfo) const
{
	if (_ovrHMDDevice)
	{
		OVR::HMDInfo ovrHMDInfo;

		_ovrHMDDevice->GetDeviceInfo(&ovrHMDInfo);

		hmdInfo.hResolution				= (float)ovrHMDInfo.HResolution;
		hmdInfo.vResolution				= (float)ovrHMDInfo.VResolution;
		hmdInfo.hScreenSize				= ovrHMDInfo.HScreenSize;
		hmdInfo.vScreenSize				= ovrHMDInfo.VScreenSize;
		hmdInfo.vScreenCenter			= ovrHMDInfo.VScreenCenter;
		hmdInfo.interpupillaryDistance	= ovrHMDInfo.InterpupillaryDistance;
		hmdInfo.lensSeparationDistance	= ovrHMDInfo.LensSeparationDistance;
		hmdInfo.eyeToScreenDistance		= ovrHMDInfo.EyeToScreenDistance;
		hmdInfo.distortionK				= Vector4::create(ovrHMDInfo.DistortionK[0], ovrHMDInfo.DistortionK[1], ovrHMDInfo.DistortionK[2], ovrHMDInfo.DistortionK[3]);
	}
	else
	{
		hmdInfo.hResolution				= 1280.0f;
		hmdInfo.vResolution				= 800.0f;
		hmdInfo.hScreenSize				= 0.14976f;
		hmdInfo.vScreenSize				= hmdInfo.hScreenSize / (1280.0f / 800.0f);
		hmdInfo.vScreenCenter			= 0.5f * hmdInfo.vScreenSize;
		hmdInfo.interpupillaryDistance	= 0.064f;
		hmdInfo.lensSeparationDistance	= 0.0635f;
		hmdInfo.eyeToScreenDistance		= 0.041f;
		hmdInfo.distortionK				= Vector4::create(1.0f, 0.22f, 0.24f, 0.0f);
	}
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
	auto context	= _sceneManager->assets()->context();

	HMDInfo hmdInfo;
	getHMDInfo(hmdInfo);

	// ffxa
	auto	pixelOffset					= Vector2::create(1.0f / TARGET_SIZE, 1.0f / TARGET_SIZE);

	// distortion scale
	const auto distortionLensShift		= 1.0f - 2.0f * hmdInfo.lensSeparationDistance / hmdInfo.hScreenSize;
	const auto fitRadius				= abs(- 1.0f - distortionLensShift);
	const auto distortionScale			= distort(fitRadius, hmdInfo.distortionK) / fitRadius;

	// projection matrix 
	const auto screenAspectRatio		= (hmdInfo.hResolution * 0.5f) / hmdInfo.vResolution;
	const auto screenFOV				= 2.0f * atanf(distortionScale * (hmdInfo.vScreenSize * 0.5f) / hmdInfo.eyeToScreenDistance);

	const auto viewCenter				= hmdInfo.hScreenSize * 0.25f;
	const auto eyeProjectionShift		= viewCenter - hmdInfo.lensSeparationDistance * 0.5f;
	const auto projectionCenterOffset	= 4.0f * eyeProjectionShift / hmdInfo.hScreenSize; // in clip coordinates

	// view transform translation in world units
	const auto halfIPD					= 0.5f * hmdInfo.interpupillaryDistance * WORLD_UNIT;

	// left eye
	render::AbstractTexture::Ptr	leftEyeTexture		= render::Texture::create(context, TARGET_SIZE, TARGET_SIZE, false, true);
	auto							leftEye				= scene::Node::create("oculusvr.leftEye");
	auto							leftRenderer		= Renderer::create();
	auto							leftPostProjection	= Matrix4x4::create()->appendTranslation(+ projectionCenterOffset, 0.0f, 0.0f);

	_leftCamera = PerspectiveCamera::create(screenAspectRatio, screenFOV, _zNear, _zFar, leftPostProjection);

	leftEyeTexture->upload();
	leftRenderer->target(leftEyeTexture);
	leftEye->addComponent(leftRenderer);
	leftEye->addComponent(_leftCamera);
	leftEye->addComponent(Transform::create(
		Matrix4x4::create()->appendTranslation(- halfIPD, 0.0f, 0.0f) // view transform
	));

	// right eye
	render::AbstractTexture::Ptr	rightEyeTexture		= render::Texture::create(context, TARGET_SIZE, TARGET_SIZE, false, true);
	auto							rightEye			= scene::Node::create("oculusvr.rightEye");
	auto							rightRenderer		= Renderer::create();
	auto							rightPostProjection	= Matrix4x4::create()->appendTranslation(- projectionCenterOffset, 0.0f, 0.0f);

	_rightCamera = PerspectiveCamera::create(screenAspectRatio, screenFOV, _zNear, _zFar, rightPostProjection);

	rightEyeTexture->upload();
	rightRenderer->target(rightEyeTexture);
	rightEye->addComponent(rightRenderer);
	rightEye->addComponent(_rightCamera);
	rightEye->addComponent(Transform::create(
		Matrix4x4::create()->appendTranslation(+ halfIPD, 0.0f, 0.0f) // view transform
	));

	_root = scene::Node::create("oculusvr");
	_root->addComponent(Transform::create());
	_root->addChild(leftEye)->addChild(rightEye);

	targets().front()->addChild(_root);

	// post processing effect
	const auto clipWidth		= 0.5f;
	const auto clipHeight		= 1.0f;

	auto leftScreenCorner		= Vector2::create(0.0f, 0.0f);
	auto leftScreenCenter		= leftScreenCorner + Vector2::create(clipWidth * 0.5f, clipHeight * 0.5f);
	auto leftLensCenter			= leftScreenCorner + Vector2::create(
		(clipWidth + distortionLensShift * 0.5f) * 0.5f,
		clipHeight * 0.5f
	);

	auto rightScreenCorner		= Vector2::create(0.5f, 0.0f);
	auto rightScreenCenter		= rightScreenCorner + Vector2::create(clipWidth * 0.5f, clipHeight * 0.5f);
	auto rightLensCenter		= rightScreenCorner + Vector2::create(
		(clipWidth - distortionLensShift * 0.5f) * 0.5f,
		clipHeight * 0.5f
	);

	auto scalePriorDistortion	= Vector2::create(2.0f / clipWidth, (2.0f / clipHeight) / screenAspectRatio);
	auto scaleAfterDistortion	= Vector2::create((clipWidth * 0.5f) / distortionScale, ((clipHeight * 0.5f) * screenAspectRatio / distortionScale));

#ifdef DEBUG_OCULUS
	std::cout << "- distortion lens shift\t= " << distortionLensShift << std::endl;
	std::cout << "- distortion scale\t= " << distortionScale << std::endl;
	std::cout << "- radial distortion\t= " << hmdInfo.distortionK->toString() << std::endl;
	std::cout << "- left lens center\t= " << leftLensCenter->toString() << "\n- left screen center\t= " << leftScreenCenter->toString() << "\n- left screen corner\t= " << leftScreenCorner->toString() << std::endl;
	std::cout << "- right lens center\t= " << rightLensCenter->toString() << "\n- right screen center\t= " << rightScreenCenter->toString() << "\n- right screen corner\t= " << rightScreenCorner->toString() << std::endl;
	std::cout << "- scale prior distortion\t= " << scalePriorDistortion->toString() << "\n- scale after distortion\t= " << scaleAfterDistortion->toString() << std::endl;
#endif // DEBUG_OCULUS

	_renderer = Renderer::create();

	auto ppFx = _sceneManager->assets()->effect("effect/OculusVR/OculusVR.effect");

	if (!ppFx)
		throw std::logic_error("OculusVR.effect has not been loaded.");

	auto ppScene = scene::Node::create()
		->addComponent(_renderer)
		->addComponent(Surface::create(
			geometry::QuadGeometry::create(_sceneManager->assets()->context()),
			material::Material::create("oculusvr")
				->set("distortionK",			hmdInfo.distortionK)
				->set("pixelOffset",			pixelOffset)
				->set("scalePriorDistortion",	scalePriorDistortion)
				->set("scaleAfterDistortion",	scaleAfterDistortion)
				->set("leftEyeTexture",			leftEyeTexture)
				->set("leftLensCenter",			leftLensCenter)
				->set("leftScreenCorner",		leftScreenCorner)
				->set("leftScreenCenter",		leftScreenCenter)
				->set("rightEyeTexture",		rightEyeTexture)
				->set("rightLensCenter",		rightLensCenter)
				->set("rightScreenCorner",		rightScreenCorner)
				->set("rightScreenCenter",		rightScreenCenter),
			ppFx
		));

	_renderEndSlot = sceneManager->renderingEnd()->connect(std::bind(
		&OculusVRCamera::renderEndHandler,
		std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
OculusVRCamera::renderEndHandler(std::shared_ptr<SceneManager>	sceneManager,
								 uint							frameId,
								 render::AbstractTexture::Ptr	renderTarget)
{
	_renderer->render(sceneManager->assets()->context());

	updateCameraOrientation();
}

/*static*/
float
OculusVRCamera::distort(float r, Vector4::Ptr distortionK)
{
	const float r2 = r * r;
	const float r4 = r2 * r2;
	const float r6 = r4 * r2;

	return r * (
		distortionK->x()
		+ r2 * distortionK->y()
		+ r4 * distortionK->z()
		+ r6 * distortionK->w()
	);
}

bool
OculusVRCamera::HMDDeviceDetected() const
{
	return _ovrHMDDevice != nullptr;
}

bool
OculusVRCamera::sensorDeviceDetected() const
{
	return _ovrSensorDevice != nullptr;
}

void
OculusVRCamera::updateCameraOrientation()
{
	if (_ovrSensorFusion == nullptr || _targetTransform == nullptr)
		return;

	const OVR::Quatf&	measurement	= _ovrSensorFusion->GetPredictedOrientation();
	auto				quaternion	= math::Quaternion::create(measurement.x, measurement.y, measurement.z, measurement.w);
	
	quaternion->toMatrix(_eyeOrientation);

	_targetTransform->matrix()->copyTranslation(_eyePosition);
	
	_targetTransform->matrix()
		->lock()
		->copyFrom(_eyeOrientation)
		->appendTranslation(_eyePosition)
		->unlock();
}

void
OculusVRCamera::resetHeadTracking()
{
	if (_ovrSensorFusion)
		_ovrSensorFusion->Reset();
}