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

#include "Container.hpp"

#include "minko/data/ArrayProvider.hpp"
#include "minko/data/Provider.hpp"

using namespace minko;
using namespace minko::data;

Container::Container() :
	std::enable_shared_from_this<Container>(),
	_providers(),
	_propertyNameToProvider(),
	_arrayLengths(data::Provider::create()),
	_propValueChanged(),
	_propReferenceChanged(),
	_propertyAddedOrRemovedSlots(),
	_providerValueChangedSlot(),
	_providerReferenceChangedSlot()
{
}

void
Container::initialize()
{
	addProvider(_arrayLengths);
}

void
Container::addProvider(std::shared_ptr<Provider> provider)
{
	assertProviderDoesNotExist(provider);

	_providers.push_back(provider);

	_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
		&Container::providerPropertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));
	
	_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
		&Container::providerPropertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));

	_providerReferenceChangedSlot[provider] = provider->propertyReferenceChanged()->connect(std::bind(
		&Container::providerReferenceChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	for (auto property : provider->values())
		providerPropertyAddedHandler(provider, property.first);
}

void
Container::addProvider(std::shared_ptr<ArrayProvider> provider)
{
	assertProviderDoesNotExist(provider);

	auto lengthPropertyName = provider->arrayName() + ".length";
	int length = _arrayLengths->hasProperty(lengthPropertyName)
		? _arrayLengths->get<int>(lengthPropertyName)
		: 0;

	provider->index(length);
	_arrayLengths->set<int>(lengthPropertyName, ++length);

	addProvider(std::dynamic_pointer_cast<Provider>(provider));
}

void
Container::removeProvider(std::shared_ptr<Provider> provider)
{
	assertProviderExists(provider);

	_providers.erase(std::find(_providers.begin(), _providers.end(), provider));
	_propertyAddedOrRemovedSlots.erase(provider);

	for (auto property : provider->values())
		_propertyNameToProvider.erase(property.first);

	if (_providerValueChangedSlot.count(provider) != 0)
		_providerValueChangedSlot.erase(provider);

	_providerValueChangedSlot.erase(provider);
}

void
Container::removeProvider(std::shared_ptr<ArrayProvider> provider)
{
	assertProviderExists(provider);

	auto lengthPropertyName = provider->arrayName() + ".length";
	int length = _arrayLengths->hasProperty(lengthPropertyName)
		? _arrayLengths->get<int>(lengthPropertyName)
		: 0;

	_arrayLengths->set<int>(lengthPropertyName, --length);

	removeProvider(std::dynamic_pointer_cast<Provider>(provider));

	if (provider->index() != length)
	{
		auto last = std::dynamic_pointer_cast<ArrayProvider>(
			*std::find(_providers.rend(), _providers.rbegin(), provider)
		);

		last->index(provider->index());
	}

	if (length == 0)
		_arrayLengths->unset(lengthPropertyName);
}

bool
Container::hasProvider(std::shared_ptr<Provider> provider)
{
	return std::find(_providers.begin(), _providers.end(), provider) != _providers.end();
}

bool
Container::hasProperty(const std::string& propertyName) const
{
	return _propertyNameToProvider.count(propertyName) != 0;
}

Container::PropertyChangedSignalPtr
Container::propertyValueChanged(const std::string& propertyName)
{
	//assertPropertyExists(propertyName);

	if (_propValueChanged.count(propertyName) == 0)
	{
		_propValueChanged[propertyName] = Signal<Container::Ptr, const std::string&>::create();

		if (_propertyNameToProvider.count(propertyName) != 0)
		{
			Provider::Ptr provider = _propertyNameToProvider[propertyName];

			if (_providerValueChangedSlot.count(provider) == 0)
				_providerValueChangedSlot[provider] = provider->propertyValueChanged()->connect(std::bind(
					&Container::providerValueChangedHandler,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2
				));
		}
	}

	return _propValueChanged[propertyName];
}

Container::PropertyChangedSignalPtr
Container::propertyReferenceChanged(const std::string& propertyName)
{
	if (_propReferenceChanged.count(propertyName) == 0)
	{
		_propReferenceChanged[propertyName] = Signal<Container::Ptr, const std::string&>::create();

		if (_propertyNameToProvider.count(propertyName) != 0)
		{
			Provider::Ptr provider = _propertyNameToProvider[propertyName];

			if (_providerReferenceChangedSlot.count(provider) == 0)
			{
				_providerReferenceChangedSlot[provider] = provider->propertyReferenceChanged()->connect(std::bind(
					&Container::providerReferenceChangedHandler,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2
				));
			}
		}
	}

	return _propReferenceChanged[propertyName];
}

void
Container::assertPropertyExists(const std::string& propertyName)
{
	if (!hasProperty(propertyName))
		throw std::invalid_argument(propertyName);	
}

void
Container::providerValueChangedHandler(Provider::Ptr			provider,
									      const std::string& 	propertyName)
{
	if (_propValueChanged.count(propertyName) != 0)
		propertyValueChanged(propertyName)->execute(shared_from_this(), propertyName);
}

void
Container::providerReferenceChangedHandler(Provider::Ptr		provider,
										   const std::string&	propertyName)
{
	propertyReferenceChanged(propertyName)->execute(shared_from_this(), propertyName);
}

void
Container::providerPropertyAddedHandler(std::shared_ptr<Provider> 	provider,
										const std::string& 			propertyName)
{
	if (_propertyNameToProvider.count(propertyName) != 0)
		throw std::logic_error("duplicate property name: " + propertyName);
	
	_propertyNameToProvider[propertyName] = provider;

	if (_propValueChanged.count(propertyName) != 0)
		_providerValueChangedSlot[provider] = provider->propertyValueChanged()->connect(std::bind(
			&Container::providerValueChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

	providerValueChangedHandler(provider, propertyName);	
}

void
Container::providerPropertyRemovedHandler(std::shared_ptr<Provider> provider,
										  const std::string&		propertyName)
{
	_propertyNameToProvider.erase(propertyName);

	if (_providerValueChangedSlot.count(provider) != 0)
		for (auto property : provider->values())
			if (_propValueChanged.count(property.first) != 0)
				return;

	_providerValueChangedSlot.erase(provider);
	_providerReferenceChangedSlot.erase(provider);

	providerValueChangedHandler(provider, propertyName);
}
