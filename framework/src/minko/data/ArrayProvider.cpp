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

#include "ArrayProvider.hpp"

using namespace minko;
using namespace minko::data;

ArrayProvider::ArrayProvider(const std::string& name, uint index) :
	_name(name),
	_index(index)
{
}

void
ArrayProvider::index(unsigned int index)
{
	if (_index == index)
		return;

	_index = index;
	for (auto& propertyNames : _propertyNameToArrayPropertyName)
		swap(propertyNames.second, _name + "[" + std::to_string(_index) + "]." + propertyNames.first);
}

bool 
ArrayProvider::hasProperty(const std::string& propertyName)
{
	return _propertyNameToArrayPropertyName.count(propertyName);
}

void
ArrayProvider::unset(const std::string& propertyName)
{
	std::string arrayPropertyName = _propertyNameToArrayPropertyName[propertyName];

	_propertyNameToArrayPropertyName.erase(propertyName);
	Provider::unset(arrayPropertyName);
}

void
ArrayProvider::registerProperty(const std::string& propertyName, std::shared_ptr<Value> value)
{
	std::string arrayPropertyName = _name + "[" + std::to_string(_index) + "]." + propertyName;
	
	_propertyNameToArrayPropertyName[propertyName] = arrayPropertyName;
	Provider::registerProperty(arrayPropertyName, value);
}
