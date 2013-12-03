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

#include "ContainerProperty.hpp"

#include "minko/data/Container.hpp"

using namespace minko;
using namespace minko::data;
			
ContainerProperty&
ContainerProperty::initialize(const data::MacroBinding&	binding, 
							  Container::Ptr			targetData, 
							  Container::Ptr			rendererData, 
							  Container::Ptr			rootData)
{
	_propertyName		= std::get<0>(binding);
	const auto& source	= std::get<1>(binding);
	_container			= _propertyName.empty() ? nullptr
		: source == data::BindingSource::TARGET && targetData->hasProperty(_propertyName) ? targetData
		: source == data::BindingSource::RENDERER && rendererData->hasProperty(_propertyName) ? rendererData
		: source == data::BindingSource::ROOT && rootData->hasProperty(_propertyName) ? rootData
		: nullptr;

	return *this;
}

/*
ContainerProperty&
ContainerProperty::initialize(const std::string&	propertyName, 
							  Container::Ptr		targetData, 
							  Container::Ptr		rendererData, 
							  Container::Ptr		rootData)
{
	_propertyName	= propertyName;

	if (targetData->hasProperty(_propertyName))
		_container	= targetData;
	else if (rendererData->hasProperty(_propertyName))
		_container	= rendererData;
	else if (rootData->hasProperty(_propertyName))
		_container	= rootData;
	else
		_container	= nullptr;

	return *this;
}
*/