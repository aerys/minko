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
		->set("transform/viewMatrix",			_view)
		->set("transform/projectionMatrix",		_projection)
		->set("transform/worldToScreenMatrix",	_viewProjection);
}

void
PerspectiveCameraController::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&PerspectiveCameraController::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&PerspectiveCameraController::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
PerspectiveCameraController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	if (targets().size() > 1)
		throw std::logic_error("PerspectiveCameraController cannot have more than 1 target.");

	target->bindings()->addProvider(_data);
	target->bindings()->propertyChanged("transform/modelToWorldMatrix")->connect(std::bind(
		&PerspectiveCameraController::localToWorldChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
PerspectiveCameraController::targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->bindings()->addProvider(_data);
}

void
PerspectiveCameraController::localToWorldChangedHandler(std::shared_ptr<DataBindings> bindings, const std::string& propertyName)
{
	std::cout << "PerspectiveCameraController::localToWorldChangedHandler()" << std::endl;

	_view->copyFrom(bindings->get<Matrix4x4::Ptr>("transform/modelToWorldMatrix"))->invert();
	_viewProjection->copyFrom(_view)->append(_projection);
}

