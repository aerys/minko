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

#include "minko/data/Provider.hpp"

#include "minko/Uuid.hpp"

using namespace minko;
using namespace minko::data;

/*static*/ const std::string Provider::NO_STRUCT_SEP = "_";

Provider::Provider(const std::string& key) :
	enable_shared_from_this(),
    _key(key),
    _uuid(minko::Uuid::getUuid()),
	_propertyAdded(Signal<Ptr, const std::string&>::create()),
    _propertyChanged(Signal<Ptr, const std::string&>::create()),
	_propertyRemoved(Signal<Ptr, const std::string&>::create())
{
}

Provider::Ptr
Provider::unset(const std::string& propertyName)
{
	const auto& formattedPropertyName = formatPropertyName(propertyName);
	
	if (_values.count(formattedPropertyName) != 0)
	{
		_values.erase(formattedPropertyName);

		_propertyRemoved->execute(shared_from_this(), formattedPropertyName);
	}

	return shared_from_this();
}

Provider::Ptr
Provider::swap(const std::string& propertyName1, const std::string& propertyName2, bool skipPropertyNameFormatting)
{
	auto formattedPropertyName1	= skipPropertyNameFormatting ? propertyName1 : formatPropertyName(propertyName1);
	auto formattedPropertyName2	= skipPropertyNameFormatting ? propertyName2 : formatPropertyName(propertyName2);
	auto hasProperty1			= hasProperty(formattedPropertyName1, true);
	auto hasProperty2			= hasProperty(formattedPropertyName2, true);

	if (!hasProperty1 && !hasProperty2)
		throw;

	if (!hasProperty1 || !hasProperty2)
	{
		auto source = hasProperty1 ? formattedPropertyName1 : formattedPropertyName2;
		auto destination = hasProperty1 ? formattedPropertyName2 : formattedPropertyName1;

		_values[destination] = _values[source];
		_values.erase(source);

		_propertyRemoved->execute(shared_from_this(), source);
		_propertyAdded->execute(shared_from_this(), destination);
        _propertyChanged->execute(shared_from_this(), destination);
	}
	else
	{
		const auto&	value1	= _values[formattedPropertyName1];
		const auto&	value2	= _values[formattedPropertyName2];
		const bool	changed = value1 != value2;

		_values[formattedPropertyName1] = value2;
		_values[formattedPropertyName2] = value1;

		if (changed)
		{
            _propertyChanged->execute(shared_from_this(), formattedPropertyName1);
            _propertyChanged->execute(shared_from_this(), formattedPropertyName2);
		}
	}

	return shared_from_this();
}

Provider::Ptr
Provider::clone()
{
	auto provider = Provider::create();
	
	provider->copyFrom(shared_from_this());

	return provider;
}

Provider::Ptr
Provider::copyFrom(Provider::Ptr source)
{
	//_names	= source->_names;
	_values = source->_values;

	return shared_from_this();
}

std::string
Provider::formatPropertyName(const std::string& propertyName) const
{
    return _key + "['" + _uuid + "']." + propertyName;
}

std::string
Provider::unformatPropertyName(const std::string& propertyName) const
{
    std::size_t pos = propertyName.find_first_of('.');
    if (pos == std::string::npos || propertyName.substr(0, pos) != _key)
        return propertyName;
    ++pos;

    return propertyName.substr(pos);
}
