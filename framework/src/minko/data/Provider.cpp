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

using namespace minko;
using namespace minko::data;

Provider::Provider() :
	enable_shared_from_this(),
	_referenceChangedSignalSlots(),
	_propertyChanged(Signal<Ptr, const std::string&>::create()),
	_referenceChanged(Signal<Ptr, const std::string&>::create()),
	_propertyAdded(Signal<Ptr, const std::string&>::create()),
	_propertyRemoved(Signal<Ptr, const std::string&>::create())
{
}

void
Provider::unset(const std::string& propertyName)
{
	_values.erase(propertyName);
	_changedSignalSlots.erase(propertyName);
	_referenceChangedSignalSlots.erase(propertyName);

	if (_values.count(propertyName) == 0)
	{
		_names.erase(std::find(_names.begin(), _names.end(), propertyName));
		_propertyRemoved->execute(shared_from_this(), propertyName);
	}
}

void
Provider::swap(const std::string& propertyName1, const std::string& propertyName2)
{
	auto hasProperty1 = hasProperty(propertyName1);
	auto hasProperty2 = hasProperty(propertyName2);

	if (!hasProperty1 && !hasProperty2)
		throw;

	if (!hasProperty1 || !hasProperty2)
	{
		auto source = hasProperty1 ? propertyName1 : propertyName2;
		auto destination = hasProperty1 ? propertyName2 : propertyName1;
		auto namesIt = std::find(_names.begin(), _names.end(), source);

		*namesIt = destination;

		_values[destination] = _values[source];
		_values.erase(source);

		_changedSignalSlots[destination] = _changedSignalSlots[source];
		_changedSignalSlots.erase(source);

		_propertyRemoved->execute(shared_from_this(), source);
		_propertyAdded->execute(shared_from_this(), destination);
	}
	else
	{
		auto value = _values[propertyName1];

		_values[propertyName1] = _values[propertyName2];
		_values[propertyName2] = value;

		_propertyChanged->execute(shared_from_this(), propertyName1);
		_propertyChanged->execute(shared_from_this(), propertyName2);
	}
}

void
Provider::registerProperty(const std::string& propertyName, std::shared_ptr<Value> value)
{
	const auto	foundValueIt	= _values.find(propertyName);
	const bool	isNewValue		= ( foundValueIt == _values.end() );
//	bool		isNewValue		= _values.count(propertyName) == 0;
	bool		valueChanged	= false;

	if (!isNewValue)
		valueChanged = !((*value) == (*foundValueIt->second));
	
	_values[propertyName] = value;
	
    _changedSignalSlots[propertyName] = value->changed()->connect(std::bind(
		&Signal<Provider::Ptr, const std::string&>::execute,
		_propertyChanged,
		shared_from_this(),
		propertyName
	));
	
	_referenceChangedSignalSlots[propertyName] = _referenceChanged->connect(std::bind(
		&Signal<Provider::Ptr, const std::string&>::execute,
		_referenceChanged,
		shared_from_this(),
		propertyName
		));

	if (isNewValue)
	{
		_names.push_back(propertyName);
		_propertyAdded->execute(shared_from_this(), propertyName);
	}
	else if (valueChanged)
	{
		_propertyChanged->execute(shared_from_this(), propertyName);
#ifdef DEBUG
		std::cout << "Provider::registerProperty\t'" << propertyName << "' changed" << std::endl;
#endif // DEBUG
	}
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

			
bool 
Provider::hasProperty(const std::string& name) const
{
	return std::find(_names.begin(), _names.end(), name) != _names.end();
}