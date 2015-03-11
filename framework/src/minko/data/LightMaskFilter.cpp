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

#include "LightMaskFilter.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Provider.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/AbstractRootDataComponent.hpp"
#include "minko/component/AbstractLight.hpp"
#include "minko/scene/Layout.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::scene;
using namespace minko::component;

/*static*/ std::vector<std::string>	LightMaskFilter::_numLightPropertyNames = initializeNumLightPropertyNames();

LightMaskFilter::LightMaskFilter():
	AbstractFilter(),
	_target(nullptr),
	_root(nullptr),
	_providerToLight(),
	_rootPropertyChangedSlots()
{
}

LightMaskFilter::Ptr
LightMaskFilter::root(Node::Ptr root)
{
	reset();

	if (root)
	{
		_root = root;

		for (auto& n : _numLightPropertyNames)
		{
			// auto slot = _root->data().propertyChanged(n).connect(
            //     [=](Store&, data::Provider::Ptr, const std::string&)
            //     {
			// 	    lightsChangedHandler();
			//     },
			//     10.f
            // );
			//
			// _rootPropertyChangedSlots.push_back(slot);
		}

		lightsChangedHandler();
	}

	return std::static_pointer_cast<LightMaskFilter>(shared_from_this());
}

void
LightMaskFilter::reset()
{
	_root = nullptr;
	_providerToLight.clear();
	_rootPropertyChangedSlots.clear();
}

bool
LightMaskFilter::operator()(Provider::Ptr data)
{
	if (_root == nullptr
		|| currentSurface() == nullptr
		|| currentSurface()->target()->root() != _root)
		return false;

#ifdef DEBUG
	assert(_root);
	assert(currentSurface());
	assert(currentSurface()->target()->root() == _root);
#endif // DEBUG

	auto foundLightIt = _providerToLight.find(data);

	if (foundLightIt == _providerToLight.end())
		return true; // the specified provider does not belong to a light

	auto surfaceLayouts	= currentSurface()->target()->layout();
	const Layout lightMask = foundLightIt->second->layoutMask();

	return (surfaceLayouts & lightMask) != 0;
}

void
LightMaskFilter::lightsChangedHandler()
{
	if (_root == nullptr)
		return;

	_layoutMaskChangedSlots.clear();
	_providerToLight.clear();

	auto withLights	= NodeSet::create(_root)
		->descendants(true)
		->where([](Node::Ptr n){ return n->hasComponent<AbstractLight>(); });

    // FIXME
	//for (auto& n : withLights->nodes())
	//{
	//	auto light = n->component<AbstractLight>();

	//	_providerToLight[light->data()] = light;

	//	_layoutMaskChangedSlots.push_back(light->data()->propertyChanged()->connect([=](Provider::Ptr provider, const std::string& lightProperty)
	//	{
	//		changed()->execute(shared_from_this(), nullptr);
	//	}));
	//}
}

/*static*/
std::vector<std::string>
LightMaskFilter::initializeNumLightPropertyNames()
{
	std::vector<std::string> names;

	names.push_back("ambientLights.length");
	names.push_back("directionalLights.length");
	names.push_back("pointLights.length");
	names.push_back("spotLights.length");

	return names;
}
