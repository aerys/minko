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

#include "minko/component/MouseManager.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/math/AbstractShape.hpp"
#include "minko/math/Box.hpp"
#include "minko/math/Ray.hpp"
#include "minko/component/MousePicking.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/component/Transform.hpp"

using namespace minko;
using namespace minko::component;

MouseManager::MouseManager(std::shared_ptr<input::Mouse> mouse) :
    _mouse(mouse),
	_ray(math::Ray::create()),
	_previousRayOrigin(0.f),
	_lastItemUnderCursor(nullptr)
{

}

void
MouseManager::targetAdded(scene::Node::Ptr target)
{
    if (!_mouse)
        return;

    _mouseMoveSlot = _mouse->move()->connect([&](MousePtr m, int dx, int dy)
    {
        // FIXME: should unproject from properties stored in data()
        auto cam = target->component<PerspectiveCamera>();

        if (cam)
            pick(_ray = cam->unproject(m->normalizedX(), m->normalizedY()));
    });
    _mouseLeftButtonDownSlot = _mouse->leftButtonDown()->connect([&](MousePtr m)
    {
        // FIXME
    });
}

void
MouseManager::targetRemoved(scene::Node::Ptr target)
{
    _mouseMoveSlot = nullptr;
    _mouseLeftButtonDownSlot = nullptr;
}

void
MouseManager::pick(std::shared_ptr<math::Ray> ray)
{
	MouseManager::HitList hits;

	auto descendants = scene::NodeSet::create(target()->root())
		->descendants(true)
		->where([&](scene::Node::Ptr node) { return node->hasComponent<BoundingBox>(); });

	std::unordered_map<scene::Node::Ptr, float> distance;
	math::Ray::Ptr localRay = math::Ray::create();

	for (auto& descendant : descendants->nodes())
	{
		auto distance = 0.f;

		if (descendant->component<BoundingBox>()->box()->cast(ray, distance))
		{
			hits.push_back(Hit(descendant, distance));

			/*
			auto transform = descendant->component<Transform>();
			uint triangleId = 0;

			if (transform)
			{
				transform->worldToModel(ray->origin(), localRay->origin());
				transform->deltaWorldToModel(ray->direction(), localRay->direction());
			}

			for (auto& surface : descendant->components<Surface>())
			{
				if (surface->geometry()->cast(transform ? localRay : ray, distance, triangleId))
				{
					hits.push_back(Hit(descendant, distance));
				}
			}
			*/
		}
	}

	hits.sort([&](Hit& a, Hit& b) { return a.second < b.second; });

	if (!hits.empty())
	{
		auto mp = hits.front().first->component<MousePicking>();

		if (_previousRayOrigin != ray->origin())
		{
			//_move->execute(shared_from_this(), hits, ray);
			if (mp)
				mp->move()->execute(mp, hits, ray);

			_previousRayOrigin = ray->origin();
		}

		//_over->execute(shared_from_this(), hits, ray);
	}
}
