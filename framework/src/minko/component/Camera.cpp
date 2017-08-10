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

#include "minko/component/Camera.hpp"

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

Camera::Camera(const math::mat4&    projection,
               const math::mat4&    postProjection) :
    _data(data::Provider::create()),
    _view(math::mat4(1.f)),
    _position(),
    _direction(0.f, 0.f, 1.f),
    _postProjection(postProjection)
{
    _data
        ->set("eyeDirection", _direction)
        ->set("eyePosition", _position)
        ->set("viewMatrix", _view)
        ->set("projectionMatrix", _projection);

    projectionMatrix(projection);
}

// TODO #Clone
/*
Camera::Camera(const Camera& camera, const CloneOption& option) :
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
Camera::clone(const CloneOption& option)
{
  auto ctrl = std::shared_ptr<Camera>(new Camera(*this, option));

  return ctrl;
}
*/

void
Camera::targetAdded(NodePtr target)
{
    target->data().addProvider(_data);

    _modelToWorldChangedSlot = target->data().propertyChanged("modelToWorldMatrix").connect(
    [&](data::Store& data, data::Provider::Ptr, const data::Provider::PropertyName&)
    {
        localToWorldChangedHandler(data);
    });

    if (target->data().hasProperty("modelToWorldMatrix"))
        updateMatrices(target->data().get<math::mat4>("modelToWorldMatrix"));
}

void
Camera::targetRemoved(NodePtr target)
{
    target->data().removeProvider(_data);
}

void
Camera::localToWorldChangedHandler(data::Store& data)
{
    updateMatrices(data.get<math::mat4>("modelToWorldMatrix"));
}

void
Camera::updateMatrices(const math::mat4& modelToWorldMatrix)
{
    _position = (modelToWorldMatrix * math::vec4(0.f, 0.f, 0.f, 1.f)).xyz();
    _direction = math::normalize(math::mat3(modelToWorldMatrix) * math::vec3(0.f, 0.f, 1.f));
    _view = math::inverse(modelToWorldMatrix);

    _data
        ->set("eyeDirection", _direction)
        ->set("eyePosition", _position)
        ->set("viewMatrix", _view);

    updateWorldToScreenMatrix();
}

void
Camera::updateWorldToScreenMatrix()
{
    _projection = _postProjection * _projection;
    _viewProjection = _projection * _view;

    _data
        ->set("projectionMatrix", _projection)
        ->set("worldToScreenMatrix", _viewProjection);
}

std::shared_ptr<math::Ray>
Camera::unproject(float x, float y)
{
    // Should take normalized X and Y coordinates (between -1 and 1)
    const auto viewport = math::vec4(-1.f, -1.f, 2.f, 2.f);

    // GLM unProject function expect coordinates with the origin at the lower left corner
    const auto unprojectedWorldPosition = math::unProject(
        math::vec3(x, -y, 0.f),
        _view,
        _projection,
        viewport
    );

    const auto rayWorldOrigin =
        math::vec3(target()->component<Transform>()->modelToWorldMatrix() *
        math::vec4(0.f, 0.f, 0.f, 1.f));
    const auto rayWorldDirection = math::normalize(unprojectedWorldPosition - rayWorldOrigin);

    return math::Ray::create(rayWorldOrigin, rayWorldDirection);
}

math::vec3
Camera::project(const math::vec3& worldPosition) const
{
    auto context = target()->root()->component<SceneManager>()->assets()->context();

    return project(
        worldPosition,
        context->viewportWidth(),
        context->viewportHeight(),
        _view,
        _viewProjection
    );
}

math::vec3
Camera::project(const math::vec3&   worldPosition,
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
