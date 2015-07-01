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

#include "minko/component/PerspectiveCamera.hpp"

#include "minko/scene/Node.hpp"
#include "minko/data/Provider.hpp"
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
	_data(data::Provider::create()),
	_fov(fov),
	_aspectRatio(aspectRatio),
	_zNear(zNear),
	_zFar(zFar),
  	_view(math::mat4(1.f)),
  	_projection(math::perspective(fov, aspectRatio, zNear, zFar)),
  	_viewProjection(_projection),
    _position(),
    _direction(0., 0., 1.f),
	_postProjection(postPerspective)
{
	_data
        ->set("eyeDirection",           _direction)
		->set("eyePosition",		    _position)
  		->set("viewMatrix",				_view)
  		->set("projectionMatrix",		_projection)
  		->set("worldToScreenMatrix",	_viewProjection)
        ->set("fov",                    _fov)
        ->set("aspectRatio",            _aspectRatio)
        ->set("zNear",                  _zNear)
        ->set("zFar",                   _zFar);
}

// TODO #Clone
/*
PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera& camera, const CloneOption& option) :
	_data(camera._data->clone()),
	_fov(camera._fov),
	_aspectRatio(camera._aspectRatio),
	_zNear(camera._zNear),
	_zFar(camera._zFar),
	_view(Matrix4x4::create()),
	_projection(Matrix4x4::create()->perspective(camera._fov, camera._aspectRatio, camera._zNear, camera._zFar)),
	_viewProjection(Matrix4x4::create()->copyFrom(_projection)),
	_position(Vector3::create()),
	_postProjection(camera._postProjection)
{
}

AbstractComponent::Ptr
PerspectiveCamera::clone(const CloneOption& option)
{
	auto ctrl = std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera(*this, option));

	return ctrl;
}
*/

void
PerspectiveCamera::targetAdded(NodePtr target)
{
	target->data().addProvider(_data);

  	_modelToWorldChangedSlot = target->data().propertyChanged("modelToWorldMatrix").connect(std::bind(
    	&PerspectiveCamera::localToWorldChangedHandler,
		std::static_pointer_cast<PerspectiveCamera>(shared_from_this()),
    	std::placeholders::_1
  	));

    if (target->data().hasProperty("modelToWorldMatrix"))
        updateMatrices(target->data().get<math::mat4>("modelToWorldMatrix"));
}

void
PerspectiveCamera::targetRemoved(NodePtr target)
{
	target->data().removeProvider(_data);
}

void
PerspectiveCamera::localToWorldChangedHandler(data::Store& data)
{
    updateMatrices(data.get<math::mat4>("modelToWorldMatrix"));
}

void
PerspectiveCamera::updateMatrices(const math::mat4& modelToWorldMatrix)
{
	_position = (modelToWorldMatrix * math::vec4(0.f, 0.f, 0.f, 1.f)).xyz();
    _direction = math::normalize(math::mat3(modelToWorldMatrix) * math::vec3(0.f, 0.f, 1.f));
    _view = math::inverse(modelToWorldMatrix);

	_data
        ->set("eyeDirection",   _direction)
		->set("eyePosition",	_position)
  		->set("viewMatrix",     _view);

	updateProjection(_fov, _aspectRatio, _zNear, _zFar);
}

void
PerspectiveCamera::updateProjection(float fov, float aspectRatio, float zNear, float zFar)
{
    _fov = fov;
    _aspectRatio = aspectRatio;
    _zNear = zNear;
    _zFar = zFar;

	_projection = _postProjection * math::perspective(fov, aspectRatio, zNear, zFar);
	_viewProjection = _projection * _view;

	_data
        ->set("fov", _fov)
        ->set("aspectRatio", _aspectRatio)
        ->set("zNear", _zNear)
        ->set("zFar", _zFar)
		->set("projectionMatrix", _projection)
		->set("worldToScreenMatrix", _viewProjection);
  }

std::shared_ptr<math::Ray>
PerspectiveCamera::unproject(float x, float y)
{
	auto fovDiv2 = _fov * .5f;
	auto dx = tanf(fovDiv2) * x * _aspectRatio;
	auto dy = -tanf(fovDiv2) * y;

	auto origin = math::vec3(dx * _zNear, dy * _zNear, -_zNear);
	auto direction = math::normalize(math::vec3(dx * _zNear, dy * _zNear, -_zNear));

	auto t = target()->component<Transform>();

	if (t)
	{
        auto tModelToWorld = t->modelToWorldMatrix();
        origin = math::vec3(tModelToWorld * math::vec4(origin, 1));
        direction = math::normalize(math::mat3(tModelToWorld) * direction).xyz();
	}

	return math::Ray::create(origin, direction);
}

math::vec3
PerspectiveCamera::project(const math::vec3& worldPosition) const
{
    auto context   = target()->root()->component<SceneManager>()->assets()->context();

    return project(
        worldPosition,
        context->viewportWidth(),
        context->viewportHeight(),
        _view,
        _viewProjection
    );
}

math::vec3
PerspectiveCamera::project(const math::vec3&   worldPosition,
                           unsigned int        viewportWidth,
                           unsigned int        viewportHeight,
                           const math::mat4&   viewMatrix,
                           const math::mat4&   viewProjectionMatrix)
{
    const auto width = viewportWidth;
    const auto height = viewportHeight;
    const auto pos = math::vec4(worldPosition, 1.f);
    auto vector = viewProjectionMatrix * pos;

    vector /= vector.w;

    return math::vec3(
       width * ((vector.x + 1.0f) * .5f),
	   height * ((1.0f - ((vector.y + 1.0f) * .5f))),
       -(viewMatrix * pos).z
    );
}
