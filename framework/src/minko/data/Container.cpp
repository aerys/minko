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

#include "minko/data/Container.hpp"

#include "minko/data/Provider.hpp"
#include "minko/data/AbstractFilter.hpp"

using namespace minko;
using namespace minko::data;

uint Container::CONTAINER_ID = 0;

Container::Container() :
	std::enable_shared_from_this<Container>(),
	_arrayLengths(data::Provider::create("fixme")),
	_propertyAdded(Container::PropertyChangedSignal::create()),
	_propertyRemoved(Container::PropertyChangedSignal::create()),
	_providerAdded(Signal<Ptr, Provider::Ptr>::create()),
	_providerRemoved(Signal<Ptr, Provider::Ptr>::create()),
	_containerId(CONTAINER_ID++)
{
}

void
Container::initialize()
{
	addProvider(_arrayLengths);
}

void
Container::addProvider(Provider::Ptr provider)
{
    //assertProviderDoesNotExist(provider);

    if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
    {
        _numUses[provider]++;
        return;
    }

    _numUses[provider] = 0;
    _providers.push_back(provider);

    _propertySlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
        &Container::providerPropertyAddedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
        &Container::providerPropertyRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyChanged()->connect(std::bind(
        &Container::providerPropertyChangedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2
    )));

    for (auto property : provider->values())
        providerPropertyAddedHandler(provider, property.first);

    _providerAdded->execute(shared_from_this(), provider);
}

void
Container::removeProvider(Provider::Ptr provider)
{
    assertProviderExists(provider);

    if (_numUses[provider] != 1)
    {
        _numUses[provider]--;
        return;
    }

    _numUses.erase(provider);

    // we have to make sure the provider is not 
    for (auto property : provider->values())
        providerPropertyRemovedHandler(provider, property.first);

    // erase all the slots (property added, changed, removed) for this provider
    _propertySlots.erase(provider);
    // destroy all signals that might have been created for each property declared by the provider
    // warning! erase the signal only if it has no callbacks anymore, otherwise it should be kept valid
    for (const auto& nameAndValue : provider->values())
        if (_propertyChanged.count(nameAndValue.first) != 0 && _propertyChanged[nameAndValue.first]->numCallbacks() == 0)
            _propertyChanged.erase(nameAndValue.first);

    _providers.erase(std::find(_providers.begin(), _providers.end(), provider));

    _providerRemoved->execute(shared_from_this(), provider);
}

bool
Container::hasProvider(std::shared_ptr<Provider> provider) const
{
	return std::find(_providers.begin(), _providers.end(), provider) != _providers.end();
}

bool
Container::hasProperty(const std::string& propertyName) const
{
	return _propertyNameToProvider.count(propertyName) != 0;
}

void
Container::assertPropertyExists(const std::string& propertyName) const
{
	if (!hasProperty(propertyName))
		throw std::invalid_argument(propertyName);	
}

void
Container::providerPropertyChangedHandler(ProviderPtr provider, const std::string& propertyName)
{
    if (_propertyChanged.count(propertyName) != 0)
        propertyChanged(propertyName)->execute(shared_from_this(), propertyName);
}

void
Container::providerPropertyAddedHandler(std::shared_ptr<Provider> 	provider,
                                        const std::string& 			propertyName)
{
    if (_propertyNameToProvider.count(propertyName) != 0)
        throw std::logic_error("duplicate property name: " + propertyName);

    _propertyNameToProvider[propertyName] = provider;
    _propertyAdded->execute(shared_from_this(), propertyName);

    providerPropertyChangedHandler(provider, propertyName);
}


void
Container::providerPropertyRemovedHandler(std::shared_ptr<Provider> provider,
                                          const std::string&		propertyName)
{

    if (_propertyNameToProvider.count(propertyName) != 0)
    {
        _propertyNameToProvider.erase(propertyName);

        if (_propertyChanged.count(propertyName) && _propertyChanged[propertyName]->numCallbacks() == 0)
            _propertyChanged.erase(propertyName);

        _propertyRemoved->execute(shared_from_this(), propertyName);
    }
}

Container::Ptr
Container::filter(const std::set<data::AbstractFilter::Ptr>&	filters,
				  Container::Ptr								output) const
{
	if (output == nullptr)
		output = data::Container::create();

	for (auto& p : _providers)
	{
		if (p == _arrayLengths)
			continue;

		bool isProviderRelevant = true;
		for (auto& f : filters)
			if (!(*f)(p))
			{
				isProviderRelevant = false;
				break;
			}

		if (isProviderRelevant && output->hasProvider(p) == false)
			output->addProvider(p);
		else if (!isProviderRelevant && output->hasProvider(p))
			output->removeProvider(p);
	}

	return output;
}
