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

#include "DataBindings.hpp"

using namespace minko::data;

DataBindings::DataBindings() :
	std::enable_shared_from_this<DataBindings>()
{
}

void
DataBindings::addProvider(std::shared_ptr<DataProvider> provider)
{
	std::list<std::shared_ptr<DataProvider>>::iterator it = std::find(
		_providers.begin(), _providers.end(), provider
	);

	if (it != _providers.end())
		throw std::invalid_argument("provider");

	_providers.push_back(provider);

	_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
		&DataBindings::dataProviderPropertyAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));
	
	_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
		&DataBindings::dataProviderPropertyRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));

	for (auto property : provider->values())
		dataProviderPropertyAddedHandler(provider, property.first);
}

void
DataBindings::removeProvider(std::shared_ptr<DataProvider> provider)
{
	std::list<std::shared_ptr<DataProvider>>::iterator it = std::find(
		_providers.begin(), _providers.end(), provider
	);

	if (it == _providers.end())
		throw std::invalid_argument("provider");

	_providers.erase(it);
	_propertyAddedOrRemovedSlots.erase(provider);

	for (auto property : provider->values())
		_propertyNameToProvider.erase(property.first);

	if (_dataProviderPropertyChangedSlot.count(provider) != 0)
		_dataProviderPropertyChangedSlot.erase(provider);

	_dataProviderPropertyChangedSlot.erase(provider);
}

bool
DataBindings::hasProvider(std::shared_ptr<DataProvider> provider)
{
	return std::find(_providers.begin(), _providers.end(), provider) != _providers.end();
}

bool
DataBindings::hasProperty(const std::string& propertyName)
{
	return _propertyNameToProvider.count(propertyName) != 0;
}

DataBindings::PropertyChangedSignalPtr
DataBindings::propertyChanged(const std::string& propertyName)
{
	//assertPropertyExists(propertyName);

	if (_propertyChanged.count(propertyName) == 0)
	{
		_propertyChanged[propertyName] = Signal<std::shared_ptr<DataBindings>, const std::string&>::create();

		if (_propertyNameToProvider.count(propertyName) != 0)
		{
			std::shared_ptr<DataProvider> provider = _propertyNameToProvider[propertyName];

			if (_dataProviderPropertyChangedSlot.count(provider) == 0)
				_dataProviderPropertyChangedSlot[provider] = provider->propertyChanged()->connect(std::bind(
					&DataBindings::dataProviderPropertyChangedHandler,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2
				));
		}
	}

	return _propertyChanged[propertyName];
}

void
DataBindings::assertPropertyExists(const std::string& propertyName)
{
	if (!hasProperty(propertyName))
		throw std::invalid_argument(propertyName);	
}

void
DataBindings::dataProviderPropertyChangedHandler(std::shared_ptr<DataProvider> 	provider,
												 const std::string& 			propertyName)
{
	if (_propertyChanged.count(propertyName) != 0)
		propertyChanged(propertyName)->execute(shared_from_this(), propertyName);
}

void
DataBindings::dataProviderPropertyAddedHandler(std::shared_ptr<DataProvider> provider,
											   const std::string& 			 propertyName)
{
	if (_propertyNameToProvider.count(propertyName) != 0)
		throw std::logic_error("Duplicate binding property name: " + propertyName);

	_propertyNameToProvider[propertyName] = provider;

	if (_propertyChanged.count(propertyName) != 0)
		_dataProviderPropertyChangedSlot[provider] = provider->propertyChanged()->connect(std::bind(
			&DataBindings::dataProviderPropertyChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

	dataProviderPropertyChangedHandler(provider, propertyName);	
}

void
DataBindings::dataProviderPropertyRemovedHandler(std::shared_ptr<DataProvider> 	provider,
												 const std::string&				propertyName)
{
	_propertyNameToProvider.erase(propertyName);

	if (_dataProviderPropertyChangedSlot.count(provider) != 0)
		for (auto property : provider->values())
			if (_propertyChanged.count(property.first) != 0)
				return;

	_dataProviderPropertyChangedSlot.erase(provider);

	dataProviderPropertyChangedHandler(provider, propertyName);
}
