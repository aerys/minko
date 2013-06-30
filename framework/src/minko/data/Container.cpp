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

using namespace minko::data;

Container::Container() :
	std::enable_shared_from_this<Container>()
{
}

void
Container::addProvider(std::shared_ptr<Provider> provider)
{
	std::list<std::shared_ptr<Provider>>::iterator it = std::find(
		_providers.begin(), _providers.end(), provider
	);

	if (it != _providers.end())
		throw std::invalid_argument("provider");

	_providers.push_back(provider);

	_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
		&Container::ProviderPropertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));
	
	_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
		&Container::ProviderPropertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));

	for (auto property : provider->values())
		ProviderPropertyAddedHandler(provider, property.first);
}

void
Container::removeProvider(std::shared_ptr<Provider> provider)
{
	std::list<std::shared_ptr<Provider>>::iterator it = std::find(
		_providers.begin(), _providers.end(), provider
	);

	if (it == _providers.end())
		throw std::invalid_argument("provider");

	_providers.erase(it);
	_propertyAddedOrRemovedSlots.erase(provider);

	for (auto property : provider->values())
		_propertyNameToProvider.erase(property.first);

	if (_ProviderPropertyChangedSlot.count(provider) != 0)
		_ProviderPropertyChangedSlot.erase(provider);

	_ProviderPropertyChangedSlot.erase(provider);
}

bool
Container::hasProvider(std::shared_ptr<Provider> provider)
{
	return std::find(_providers.begin(), _providers.end(), provider) != _providers.end();
}

bool
Container::hasProperty(const std::string& propertyName)
{
	return _propertyNameToProvider.count(propertyName) != 0;
}

Container::PropertyChangedSignalPtr
Container::propertyChanged(const std::string& propertyName)
{
	//assertPropertyExists(propertyName);

	if (_propertyChanged.count(propertyName) == 0)
	{
		_propertyChanged[propertyName] = Signal<std::shared_ptr<Container>, const std::string&>::create();

		if (_propertyNameToProvider.count(propertyName) != 0)
		{
			std::shared_ptr<Provider> provider = _propertyNameToProvider[propertyName];

			if (_ProviderPropertyChangedSlot.count(provider) == 0)
				_ProviderPropertyChangedSlot[provider] = provider->propertyChanged()->connect(std::bind(
					&Container::ProviderPropertyChangedHandler,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2
				));
		}
	}

	return _propertyChanged[propertyName];
}

void
Container::assertPropertyExists(const std::string& propertyName)
{
	if (!hasProperty(propertyName))
		throw std::invalid_argument(propertyName);	
}

void
Container::ProviderPropertyChangedHandler(std::shared_ptr<Provider> 	provider,
												 const std::string& 			propertyName)
{
	if (_propertyChanged.count(propertyName) != 0)
		propertyChanged(propertyName)->execute(shared_from_this(), propertyName);
}

void
Container::ProviderPropertyAddedHandler(std::shared_ptr<Provider> provider,
											   const std::string& 			 propertyName)
{
	if (_propertyNameToProvider.count(propertyName) != 0)
		throw std::logic_error("Duplicate binding property name: " + propertyName);

	_propertyNameToProvider[propertyName] = provider;

	if (_propertyChanged.count(propertyName) != 0)
		_ProviderPropertyChangedSlot[provider] = provider->propertyChanged()->connect(std::bind(
			&Container::ProviderPropertyChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

	ProviderPropertyChangedHandler(provider, propertyName);	
}

void
Container::ProviderPropertyRemovedHandler(std::shared_ptr<Provider> 	provider,
												 const std::string&				propertyName)
{
	_propertyNameToProvider.erase(propertyName);

	if (_ProviderPropertyChangedSlot.count(provider) != 0)
		for (auto property : provider->values())
			if (_propertyChanged.count(property.first) != 0)
				return;

	_ProviderPropertyChangedSlot.erase(provider);

	ProviderPropertyChangedHandler(provider, propertyName);
}
