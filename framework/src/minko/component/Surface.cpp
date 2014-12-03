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

#include "minko/component/Surface.hpp"

#include "minko/scene/Node.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/Program.hpp"
#include "minko/data/Store.hpp"
#include "minko/data/Provider.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/CloneOption.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::data;
using namespace minko::component;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::material;

const std::string Surface::SURFACE_COLLECTION_NAME = "surface";
const std::string Surface::GEOMETRY_COLLECTION_NAME = "geometry";
const std::string Surface::MATERIAL_COLLECTION_NAME = "material";
const std::string Surface::EFFECT_COLLECTION_NAME = "effect";

Surface::Surface(std::string		name,
				 Geometry::Ptr 		geometry,
				 Material::Ptr 		material,
				 Effect::Ptr		effect,
				 const std::string&	technique) :
	AbstractComponent(),
	_name(name),
	_geometry(geometry),
	_material(material),
	_effect(effect),
    _provider(data::Provider::create()),
	_technique(technique)
{
	if (_effect == nullptr)
		throw std::invalid_argument("effect");
	if (!_effect->hasTechnique(_technique))
		throw std::logic_error("Effect does not provide a '" + _technique + "' technique.");
}

Surface::Surface(const Surface& surface, const CloneOption& option) :
	AbstractComponent(surface, option),
	_name(surface._name),
	_geometry(surface._geometry), //needed for skinning: option == CloneOption::SHALLOW ? surface._geometry : surface._geometry->clone()
	_material(option == CloneOption::SHALLOW ? surface._material : std::static_pointer_cast<Material>(surface._material->clone())),
	_effect(surface._effect),
	_technique(surface._technique),
	_visible(surface._visible),
	_rendererToVisibility(surface._rendererToVisibility),
	_rendererToComputedVisibility(surface._rendererToComputedVisibility),
	_techniqueChanged(TechniqueChangedSignal::create()),
	_visibilityChanged(VisibilityChangedSignal::create()),
	_computedVisibilityChanged(VisibilityChangedSignal::create()),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr)
{
	if (_effect == nullptr)
		throw std::invalid_argument("effect");
	if (!_effect->hasTechnique(_technique))
		throw std::logic_error("Effect does not provide a '" + _technique + "' technique.");
}

AbstractComponent::Ptr
Surface::clone(const CloneOption& option)
{
	Ptr surface(new Surface(*this, option));

	return surface;
}

void
Surface::targetAdded(scene::Node::Ptr target)
{
    auto& targetData = target->data();

    targetData.addProvider(_provider, SURFACE_COLLECTION_NAME);
    targetData.addProvider(_material->data(), MATERIAL_COLLECTION_NAME);
    targetData.addProvider(_geometry->data(), GEOMETRY_COLLECTION_NAME);
    targetData.addProvider(_effect->data(), EFFECT_COLLECTION_NAME);
}

void
Surface::targetRemoved(scene::Node::Ptr target)
{
    auto& targetData = target->data();

    targetData.removeProvider(_provider, SURFACE_COLLECTION_NAME);
    targetData.removeProvider(_material->data(), MATERIAL_COLLECTION_NAME);
    targetData.removeProvider(_geometry->data(), GEOMETRY_COLLECTION_NAME);
    targetData.removeProvider(_effect->data(), EFFECT_COLLECTION_NAME);
}

void
Surface::geometry(geometry::Geometry::Ptr geometry)
{
    if (geometry == _geometry)
        return;

    auto t = target();

    if (t)
        t->data().removeProvider(_geometry->data(), GEOMETRY_COLLECTION_NAME);

    _geometry = geometry;

    if (t)
        t->data().addProvider(_geometry->data(), GEOMETRY_COLLECTION_NAME);

    _geometryChanged.execute(std::static_pointer_cast<Surface>(shared_from_this()));
}

void
Surface::firstIndex(unsigned short index)
{
    // TODO
}

void
Surface::numIndices(unsigned short numIndices)
{
    // TODO
}

void
Surface::material(material::Material::Ptr material)
{
    if (material == _material)
        return;

    auto t = target();

    if (t)
        t->data().removeProvider(_material->data(), MATERIAL_COLLECTION_NAME);

    _material = material;

    if (t)
        t->data().addProvider(_material->data(), MATERIAL_COLLECTION_NAME);

    _materialChanged.execute(std::static_pointer_cast<Surface>(shared_from_this()));
}

void
Surface::effect(render::Effect::Ptr		effect, 
				const std::string&		technique)
{
	setEffectAndTechnique(effect, technique);
}

void
Surface::setEffectAndTechnique(Effect::Ptr			effect,
							   const std::string&	technique)
{
	if (effect == nullptr)
		throw std::invalid_argument("effect");
	if (!effect->hasTechnique(technique))
		throw std::logic_error("Effect does not provide a '" + technique + "' technique.");

    auto changed = false;

    if (effect != _effect)
    {
        changed = true;
	    target()->data().removeProvider(_effect->data(), EFFECT_COLLECTION_NAME);
    	_effect = effect;
        target()->data().addProvider(effect->data(), EFFECT_COLLECTION_NAME);
    }

    if (technique != _technique)
    {
        changed = true;
    	_technique = technique;
        _provider->set("technique", technique);
    }

    if (changed)
        _effectChanged.execute(std::static_pointer_cast<Surface>(shared_from_this()));
}