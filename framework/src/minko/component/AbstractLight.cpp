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

#include "minko/component/AbstractLight.hpp"

#include "minko/Color.hpp"
#include "minko/scene/Node.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"

using namespace minko;
using namespace minko::component;

AbstractLight::AbstractLight(const std::string& arrayName) :
	AbstractRootDataComponent<data::ArrayProvider>(data::ArrayProvider::create(arrayName)),
	_color(math::Vector3::create(1.0f, 1.0f, 1.0f)),
	_targetLayoutChangedSlot(nullptr)
{
	data()
		->set("color",	_color)
		->set("mask",	scene::Layout::Mask::EVERYTHING);
}

AbstractLight::Ptr
AbstractLight::color(math::Vector3::Ptr color)
{
	_color->copyFrom(color);

	return std::static_pointer_cast<AbstractLight>(shared_from_this());
}

AbstractLight::Ptr
AbstractLight::color(math::Vector4::Ptr color)
{
	_color->setTo(color->x(), color->y(), color->z());

	return std::static_pointer_cast<AbstractLight>(shared_from_this());
}

AbstractLight::Ptr
AbstractLight::color(uint rgba)
{
	return color(Color::uintToVec4(rgba));
}

void
AbstractLight::targetAddedHandler(component::AbstractComponent::Ptr component, 
								  scene::Node::Ptr					target)
{
	AbstractRootDataComponent<data::ArrayProvider>::targetAddedHandler(component, target);

	_targetLayoutChangedSlot = target->layoutsChanged()->connect(std::bind(
		&AbstractLight::targetLayoutsChangedHandler,
		std::static_pointer_cast<AbstractLight>(shared_from_this()),
		std::placeholders::_1,
		target
	));

	targetLayoutsChangedHandler(nullptr, target);
}

void
AbstractLight::targetRemovedHandler(component::AbstractComponent::Ptr component,
									scene::Node::Ptr					target)
{
	AbstractRootDataComponent<data::ArrayProvider>::targetRemovedHandler(component, target);

	_targetLayoutChangedSlot = nullptr;
}

void
AbstractLight::targetLayoutsChangedHandler(scene::Node::Ptr, scene::Node::Ptr target)
{
	if (target)
		data()->set("mask", target->layouts());
}