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

#include "Provider.hpp"

using namespace minko::data;

void
Provider::unset(const std::string& propertyName)
{
	_values.erase(propertyName);
	_changedSignalSlots.erase(propertyName);

	if (_values.count(propertyName) == 0)
	{
		_names.erase(std::find(_names.begin(), _names.end(), propertyName));
		_propertyRemoved->execute(shared_from_this(), propertyName);
	}
}

void
Provider::registerProperty(const std::string& propertyName, std::shared_ptr<Value> value)
{
	bool isNewValue = _values.count(propertyName) == 0;

	_values[propertyName] = value;
    _changedSignalSlots[propertyName] = value->changed()->connect(std::bind(
		&Signal<Provider::Ptr, const std::string&>::execute,
		_propertyChanged,
		shared_from_this(),
		propertyName
	));

	if (isNewValue)
	{
		_names.push_back(propertyName);
		_propertyAdded->execute(shared_from_this(), propertyName);
	}
	else
		_propertyChanged->execute(shared_from_this(), propertyName);
}


void
Provider::propertyWrapperInitHandler(const std::string& propertyName)
{
	_propertyAdded->execute(shared_from_this(), propertyName);

	/*_values[propertyName] = Value::create(std::bind(
		&Signal<ptr, const std::string&>::execute,
		_propertyChanged,
		shared_from_this(),
		propertyName
	));*/
}
