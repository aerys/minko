#include "PerspectiveCameraController.hpp"
#include "minko/scene/Node.hpp"

using namespace minko::scene::controller;

PerspectiveCameraController::PerspectiveCameraController(float fov, float aspectRatio, float zNear, float zFar) :
	_view(Matrix4x4::create()),
	_projection(Matrix4x4::create()->perspective(fov, aspectRatio, zNear, zFar)),
	_viewProjection(Matrix4x4::create()->copyFrom(_projection)),
	_data(DataProvider::create())
{
	_data
		->setProperty("transform/viewMatrix",			_view)
		->setProperty("transform/projectionMatrix",		_projection)
		->setProperty("transfrom/viewProjectionMatrix",	_viewProjection);
}

void
PerspectiveCameraController::initialize()
{
	_targetAddedCd = targetAdded()->add(std::bind(
		&PerspectiveCameraController::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));

	_targetRemovedCd = targetRemoved()->add(std::bind(
		&PerspectiveCameraController::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
PerspectiveCameraController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	if (targets().size() > 1)
		throw std::logic_error("PerspectiveCameraController cannot have more than 1 target.");

	target->bindings()->addProvider(_data);
}

void
PerspectiveCameraController::targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->bindings()->addProvider(_data);
}

/*void
PerspectiveCameraController::localToWorldChangedHandler()
{
	// FIXME: _view->copyFrom(localToWorld)->invert();
}*/
