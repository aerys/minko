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

#include "minko/data/Provider.hpp"

#include "minko/Uuid.hpp"

#ifdef MINKO_USE_SPARSE_HASH_MAP
# include "sparsehash/sparse_hash_map"
#endif

using namespace minko;
using namespace minko::data;

Provider::Provider() :
    Uuid::enable_uuid(),
    _values(new ValueMap())
{
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _values->set_deleted_key("");
#endif
}

Provider::Provider(const std::string& uuid) :
    Uuid::enable_uuid(uuid),
    _values(new ValueMap())
{
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _values->set_deleted_key("");
#endif
}

Provider::Provider(const DefaultValueMap& values) :
	_values(new ValueMap())
{
    for (auto& p : values)
        setValue(p.first, p.second);
}

Provider::~Provider()
{
    delete _values;
}

Provider::Ptr
Provider::set(std::initializer_list<data::Provider::ValueType> values)
{
    for (auto& p : values)
        setValue(p.first, new Any(p.second));

	return shared_from_this();
}

Provider::Ptr
Provider::unset(const std::string& propertyName)
{
    auto propertyIt = _values->find(propertyName);

    if (propertyIt != _values->end())
	{
        delete propertyIt->second;

        _values->erase(propertyName);
		_propertyRemoved.execute(shared_from_this(), propertyName);
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
    _values->clear();

    for (auto nameAndValue : *source->_values)
        _values->insert({ nameAndValue.first, new Any(*nameAndValue.second) });

	return shared_from_this();
}

void
Provider::merge(Provider::Ptr source)
{
    for (auto nameAndValue : *source->_values)
        (*_values)[nameAndValue.first] = nameAndValue.second;
}

Any*
Provider::getValue(const PropertyName& propertyName) const
{
    const auto foundIt = _values->find(propertyName);

    if (foundIt == _values->end())
	    throw std::invalid_argument("propertyName");

    return foundIt->second;
}

void
Provider::setValue(const PropertyName& propertyName, Any* value)
{
    (*_values)[propertyName] = value;
}

bool
Provider::hasProperty(const PropertyName& propertyName) const
{
    return _values->count(propertyName) != 0;
}
