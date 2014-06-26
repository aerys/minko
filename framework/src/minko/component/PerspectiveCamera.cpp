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
#include "minko/data/StructureProvider.hpp"
#include "minko/math/Ray.hpp"
#include "minko/component/Transform.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::component;

PerspectiveCamera::PerspectiveCamera(float			      fov,
                                     float			      aspectRatio,
                                     float			      zNear,
                                     float			      zFar,
									 const math::mat4&	  postPerspective) :
	_data(data::StructureProvider::create("camera")),
	_fov(fov),
	_aspectRatio(aspectRatio),
	_zNear(zNear),
	_zFar(zFar),
  	_view(math::mat4(1.f)),
  	_projection(math::perspective(fov, aspectRatio, zNear, zFar)),
  	_viewProjection(_projection),
    _position(),
	_postProjection(postPerspective)
{
}

void
PerspectiveCamera::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&PerspectiveCamera::targetAddedHandler,
		std::static_pointer_cast<PerspectiveCamera>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&PerspectiveCamera::targetRemovedHandler,
		std::static_pointer_cast<PerspectiveCamera>(shared_from_this()),
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
		std::static_pointer_cast<PerspectiveCamera>(shared_from_this()),
    	std::placeholders::_1,
    	std::placeholders::_2
  	));

    if (target->data()->hasProperty("transform.modelToWorldMatrix"))
        updateMatrices(target->data()->get<math::mat4>("transform.modelToWorldMatrix"));
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
    updateMatrices(data->get<math::mat4>("transform.modelToWorldMatrix"));
}

void
PerspectiveCamera::updateMatrices(const math::mat4& modelToWorldMatrix)
{
	_position = (modelToWorldMatrix * math::vec4(0.f, 0.f, 0.f, 1.f)).xyz();
    _view = math::inverse(modelToWorldMatrix);
    
	_data
		->set("position",	_position)
  		->set("viewMatrix", _view);

	updateProjection(_fov, _aspectRatio, _zNear, _zFar);
}

void
PerspectiveCamera::updateProjection(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
	_projection = _postProjection * math::perspective(fieldOfView, aspectRatio, zNear, zFar);
	_viewProjection = _projection * _view;

	_data
		->set("projectionMatrix",		_projection)
  		->set("worldToScreenMatrix",	_viewProjection);
}

std::shared_ptr<math::Ray>
PerspectiveCamera::unproject(float x, float y)
{
	auto fovDiv2 = _fov * .5f;
	auto dx = tanf(fovDiv2) * x * _aspectRatio;
	auto dy = -tanf(fovDiv2) * y;

	auto origin = math::vec3(dx * _zNear, dy * _zNear, -_zNear);
	auto direction = math::normalize(math::vec3(dx * _zNear, dy * _zNear, -_zNear));

	auto t = targets()[0]->component<Transform>();

	if (t)
	{
		origin = (math::vec4(origin, 1.f) * t->modelToWorldMatrix()).xyz();
		direction = math::normalize(direction * math::mat3x3(t->modelToWorldMatrix()));
	}

	return math::Ray::create(origin, direction);
}

math::vec3
PerspectiveCamera::project(math::vec3 worldPosition)
{
    auto context   = getTarget(0)->root()->component<SceneManager>()->assets()->context();
    auto width     = context->viewportWidth();
    auto height    = context->viewportHeight();
    auto pos       = math::vec4(worldPosition, 1.f);
    auto vector    = _viewProjection * pos;
    
    vector /= vector.w;

    return {
       width * ((vector.x + 1.0f) * .5f),
	   height * ((1.0f - ((vector.y + 1.0f) * .5f))),
       -(_view * pos).z
    };
}
