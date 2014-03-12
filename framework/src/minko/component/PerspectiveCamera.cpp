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

#include "minko/component/PerspectiveCamera.hpp"

#include "minko/scene/Node.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/math/Ray.hpp"
#include "minko/component/Transform.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

PerspectiveCamera::PerspectiveCamera(float			fov,
                                     float			aspectRatio,
                                     float			zNear,
                                     float			zFar,
									 Matrix4x4::Ptr	postPerspective) :
	_data(data::StructureProvider::create("camera")),
	_fov(fov),
	_aspectRatio(aspectRatio),
	_zNear(zNear),
	_zFar(zFar),
  	_view(Matrix4x4::create()),
  	_projection(Matrix4x4::create()->perspective(fov, aspectRatio, zNear, zFar)),
  	_viewProjection(Matrix4x4::create()->copyFrom(_projection)),
    _position(Vector3::create()),
	_postProjection(postPerspective)
{
}

void
PerspectiveCamera::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&PerspectiveCamera::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&PerspectiveCamera::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_data
		->set("position",				_position)
  		->set("viewMatrix",				_view)
  		->set("projectionMatrix",		_projection)
  		->set("worldToScreenMatrix",	_viewProjection);
}

void
PerspectiveCamera::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	target->data()->addProvider(_data);

  	_modelToWorldChangedSlot = target->data()->propertyValueChanged("transform.modelToWorldMatrix")->connect(std::bind(
    	&PerspectiveCamera::localToWorldChangedHandler,
		shared_from_this(),
    	std::placeholders::_1,
    	std::placeholders::_2
  	));

    if (target->data()->hasProperty("transform.modelToWorldMatrix"))
        updateMatrices(target->data()->get<Matrix4x4::Ptr>("transform.modelToWorldMatrix"));
}

void
PerspectiveCamera::targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	target->data()->removeProvider(_data);
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
	_view->lock();
  	_view->copyFrom(modelToWorldMatrix);
    _view->transform(Vector3::zero(), _position);
    _view->invert();
	_view->unlock();

	updateProjection(_fov, _aspectRatio, _zNear, _zFar);
}

void
PerspectiveCamera::updateProjection(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
	_projection->perspective(_fov, _aspectRatio, _zNear, _zFar);

	if (_postProjection)
		_projection->append(_postProjection);

	_viewProjection->lock()->copyFrom(_view)->append(_projection)->unlock();
}

std::shared_ptr<math::Ray>
PerspectiveCamera::unproject(float x, float y, std::shared_ptr<math::Ray> out)
{
	if (!out)
		out = Ray::create();

	auto fovDiv2 = _fov * .5f;
	auto dx = tanf(fovDiv2) * x * _aspectRatio;
	auto dy = -tanf(fovDiv2) * y;

	out->origin()->setTo(dx * _zNear, dy * _zNear, -_zNear);
	out->direction()->setTo(dx * _zNear, dy * _zNear, -_zNear)->normalize();

	auto t = targets()[0]->component<Transform>();

	if (t)
	{
		t->modelToWorld(out->origin(), out->origin());
		t->deltaModelToWorld(out->direction(), out->direction());
		out->direction()->normalize();
	}

	return out;
}
