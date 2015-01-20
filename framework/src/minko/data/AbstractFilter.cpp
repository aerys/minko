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

#include "minko/data/AbstractFilter.hpp"

#include "minko/scene/Node.hpp"
#include "minko/component/Surface.hpp"
#include "minko/data/Store.hpp"

using namespace minko;
using namespace minko::data;

AbstractFilter::Ptr
AbstractFilter::currentSurface(component::Surface::Ptr value)
{
    /*
	_currentSurface				= value;
	_currentSurfaceRemovedSlot	= _currentSurface->targetRemoved()->connect([=](AbsCmpPtr c, NodePtr n){ 
		currentSurfaceRemovedHandler(c, n); 
	});
	_currentSurfaceTargetRemovedSlot = _currentSurface->targets().front()->removed()->connect([=](NodePtr n, NodePtr t, NodePtr a){
		currentSurfaceTargetRemovedHandler(n, t, a);
	});

	auto targets = value->targets();

	for (auto target : targets)
	{
		if (_surfaceTargetPropertyChangedSlots.find(target) == _surfaceTargetPropertyChangedSlots.end())
		{
			for (auto propertyName : _watchedProperties)
			{
				_surfaceTargetPropertyChangedSlots[target].push_back(target->data()->propertyChanged(propertyName)->connect(
					[=](Store::Ptr, const std::string&, const std::string&)
					{
						changed()->execute(shared_from_this(), value);
					}
		        ));
			}
		}
	}
    */

	return shared_from_this();
}

void
AbstractFilter::watchProperty(const std::string propertyName)
{
	_watchedProperties.push_back(propertyName);

	//_surfaceTargetPropertyChangedSlots.clear();

	changed()->execute(shared_from_this(), nullptr);
}

void
AbstractFilter::unwatchProperty(const std::string propertyName)
{
	auto it = std::find(_watchedProperties.begin(), _watchedProperties.end(), propertyName);

	if (it == _watchedProperties.end())
		std::invalid_argument("This property is not watching currently.");

	_watchedProperties.remove(propertyName);
	//_surfaceTargetPropertyChangedSlots.clear();

	changed()->execute(shared_from_this(), nullptr);
}