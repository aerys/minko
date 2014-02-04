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

#include "minko/component/Surface.hpp"

#include "minko/scene/Node.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/Program.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/ArrayProvider.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::component;
using namespace minko::geometry;
using namespace minko::render;

Surface::Surface(Geometry::Ptr 				geometry,
				 data::Provider::Ptr 		material,
				 Effect::Ptr				effect,
				 const std::string&			technique) :
	AbstractComponent(),
	_geometry(geometry),
	_material(material),
	_effect(effect),
	_technique(technique),
	_techniqueChanged(TechniqueChangedSignal::create()),
	_visibilityChanged(VisibilityChangedSignal::create()),
	_computedVisibilityChanged(VisibilityChangedSignal::create()),
	_geometryId(-1),
	_materialId(-1)
{
}

void
Surface::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Surface::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Surface::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_dataProviderIndexChangedSlots.push_back(_geometry->data()->indexChanged()->connect(std::bind(
		&Surface::geometryProviderIndexChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		), 10.f));

	auto arrayProviderMaterial = std::dynamic_pointer_cast<ArrayProvider>(_material);

	if (arrayProviderMaterial)
	_dataProviderIndexChangedSlots.push_back(arrayProviderMaterial->indexChanged()->connect(std::bind(
		&Surface::materialProviderIndexChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		), 10.f));

	if (_effect->techniques().count(_technique) == 0)
		throw std::logic_error("The technique '" + _technique + "' does not exist.");
}

void
Surface::visible(bool value)
{
	if (_visible != value)
	{
		_visible = value;
		_visibilityChanged->execute(shared_from_this(), value);
	}
}

void
Surface::computedVisibility(bool value)
{
	if (_computedVisibility != value)
	{
		_computedVisibility = value;
		_computedVisibilityChanged->execute(shared_from_this(), value);
	}
}

void
Surface::geometry(std::shared_ptr<geometry::Geometry> newGeometry)
{
	for (unsigned int i = 0; i < targets().size(); ++i)
	{
		std::shared_ptr<scene::Node> target = targets()[i];

		target->data()->removeProvider(_geometry->data());
		target->data()->addProvider(newGeometry->data());
	}

	_geometry = newGeometry;
}

void
Surface::targetAddedHandler(AbstractComponent::Ptr	ctrl,
							scene::Node::Ptr		target)

{
	auto targetData	= target->data();

	_removedSlot = target->removed()->connect(std::bind(
		&Surface::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	targetData->addProvider(std::dynamic_pointer_cast<data::ArrayProvider>(_material));
	_materialId = std::dynamic_pointer_cast<data::ArrayProvider>(_material)->index();
	targetData->addProvider(_geometry->data());
	_geometryId = _geometry->data()->index();
	targetData->addProvider(_effect->data());
}

void
Surface::removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
}

void
Surface::targetRemovedHandler(AbstractComponent::Ptr	ctrl,
							  scene::Node::Ptr			target)
{
	auto data = target->data();

	_removedSlot	= nullptr;
	
	data->removeProvider(_material);
	data->removeProvider(_geometry->data());
	data->removeProvider(_effect->data());

	_materialId = -1;
	_geometryId = -1;
}

void
Surface::setTechnique(const std::string&	technique,
					  bool					updateDrawcalls)
{
	if (_technique == technique)
		return;

#ifdef DEBUG_FALLBACK
	std::cout << "surf[" << this << "]\tchange technique\t'" << _technique << "'\t-> '" << technique << "'" << std::endl;
#endif // DEBUG_FALLBACK

	_technique = technique;

	if (!_effect->hasTechnique(_technique))
		throw std::logic_error("The technique '" + _technique + "' does not exist.");

	_techniqueChanged->execute(shared_from_this(), _technique, updateDrawcalls);
}

void
Surface::geometryProviderIndexChanged(ArrayProviderPtr arrayProvider, uint index)
{
	_geometryId = index;
}

void
Surface::materialProviderIndexChanged(ArrayProviderPtr arrayProvider, uint index)
{
	_materialId = index;
}