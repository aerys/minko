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

#include "minko/component/MousePicking.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/math/AbstractShape.hpp"
#include "minko/math/Ray.hpp"

using namespace minko;
using namespace minko::component;

MousePicking::MousePicking() :
	AbstractComponent(),
	_move(MouseSignal::create()),
	_over(MouseSignal::create()),
	_out(MouseSignal::create()),
	_rollOver(MouseSignal::create()),
	_rollOut(MouseSignal::create()),
	_leftButtonUp(MouseSignal::create()),
	_leftButtonDown(MouseSignal::create()),
	_previousRayOrigin(0.f)
{

}

void
MousePicking::initialize()
{
}

void
MousePicking::pick(std::shared_ptr<math::Ray>	ray)
{
	MousePicking::HitList hits;

	auto descendants = scene::NodeSet::create(target())
		->descendants(true)
		->where([&](scene::Node::Ptr node) 
		{ 
			return (node->layout() & layoutMask()) != 0 
				&&  node->hasComponent<BoundingBox>(); 
		}
	);
	
	std::unordered_map<scene::Node::Ptr, float> distance;

	for (auto& descendant : descendants->nodes())
		for (auto& box : descendant->components<BoundingBox>())
		{
			auto distance = 0.f;

			if (box->shape()->cast(ray, distance))
				hits.push_back(Hit(descendant, distance));
		}

	hits.sort([&](Hit& a, Hit& b) { return a.second < b.second; });

	if (!hits.empty())
	{
		if (_previousRayOrigin == ray->origin())
		{
			_move->execute(
				std::static_pointer_cast<MousePicking>(shared_from_this()), 
				hits, 
				ray
			);
			_previousRayOrigin = ray->origin();
		}

		_over->execute(
			std::static_pointer_cast<MousePicking>(shared_from_this()), 
			hits, 
			ray
		);
	}
}
