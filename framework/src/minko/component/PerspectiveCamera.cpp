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

#include "PerspectiveCamera.hpp"

#include "minko/scene/Node.hpp"
#include "minko/component/Surface.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko::component;
using namespace minko::math;

PerspectiveCamera::PerspectiveCamera(float fov,
                                     float aspectRatio,
                                     float zNear,
                                     float zFar) :
    _enabled(true),
	_view(Matrix4x4::create()),
	_projection(Matrix4x4::create()->perspective(fov, aspectRatio, zNear, zFar)),
	_viewProjection(Matrix4x4::create()->copyFrom(_projection)),
    _position(Vector3::create())
{
	_data
        ->set("camera.position",            _position)
		->set("camera.viewMatrix",			_view)
		->set("camera.projectionMatrix",	_projection)
		->set("camera.worldToScreenMatrix",	_viewProjection);
}

void
PerspectiveCamera::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
    AbstractRootDataComponent::targetAddedHandler(ctrl, target);

	_modelToWorldChangedSlot = target->data()->propertyChanged("transform.modelToWorldMatrix")->connect(std::bind(
		&PerspectiveCamera::localToWorldChangedHandler,
        std::dynamic_pointer_cast<PerspectiveCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	));

    if (target->data()->hasProperty("transform.modelToWorldMatrix"))
        updateMatrices(target->data()->get<Matrix4x4::Ptr>("transform.modelToWorldMatrix"));
}

void
PerspectiveCamera::localToWorldChangedHandler(data::Container::Ptr	data,
											  const std::string&	propertyName)
{
    updateMatrices(data->get<Matrix4x4::Ptr>("transform.modelToWorldMatrix"));
}

void
PerspectiveCamera::updateMatrices(std::shared_ptr<Matrix4x4> modelToWorldMatrix)
{
  	_view->copyFrom(modelToWorldMatrix);
    _view->transform(Vector3::zero(), _position);
    _view->invert();

    _viewProjection->copyFrom(_view)->append(_projection);
}
