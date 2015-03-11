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

#include "minko/component/AbstractDiscreteLight.hpp"

#include "minko/scene/Node.hpp"
#include "minko/data/Store.hpp"

using namespace minko;
using namespace minko::component;

AbstractDiscreteLight::AbstractDiscreteLight(const std::string& arrayName,
											 float				diffuse,
											 float				specular) :
	AbstractLight(arrayName)
{
	data()
		->set("diffuse", 	diffuse)
		->set("specular", 	specular);
}

void
AbstractDiscreteLight::targetAdded(std::shared_ptr<scene::Node> target)
{
	AbstractLight::targetAdded(target);

	_modelToWorldChangedSlot = target->data().propertyChanged("modelToWorldMatrix").connect(std::bind(
		&AbstractDiscreteLight::modelToWorldMatrixChangedHandler,
		std::static_pointer_cast<AbstractDiscreteLight>(shared_from_this()),
		std::placeholders::_1,
        std::placeholders::_3
	));

	if (target->data().hasProperty("modelToWorldMatrix"))
		updateModelToWorldMatrix(target->data().get<math::mat4>("modelToWorldMatrix"));
}

void
AbstractDiscreteLight::targetRemoved(std::shared_ptr<scene::Node> target)
{
	AbstractLight::targetRemoved(target);

	_modelToWorldChangedSlot = nullptr;
}

void
AbstractDiscreteLight::modelToWorldMatrixChangedHandler(data::Store& 						container,
								 						const data::Store::PropertyName& 	propertyName)
{
	updateModelToWorldMatrix(container.get<math::mat4>(propertyName));
}
