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
#include "minko/data/Collection.hpp"
#include "minko/data/AbstractFilter.hpp"

using namespace minko;
using namespace minko::data;

Container::Container() :
	std::enable_shared_from_this<Container>(),
	_propertyAdded(Container::PropertyChangedSignal::create()),
	_propertyRemoved(Container::PropertyChangedSignal::create()),
	_providerAdded(Signal<Ptr, Provider::Ptr>::create()),
	_providerRemoved(Signal<Ptr, Provider::Ptr>::create()),
    _collectionAdded(Signal<Ptr, Collection::Ptr>::create()),
    _collectionRemoved(Signal<Ptr, Collection::Ptr>::create())
{
}

void
Container::addCollection(std::shared_ptr<Collection> collection)
{
    _collections.push_back(collection);

    _collectionItemAddedSlots[collection] = collection->itemAdded() += std::bind(
        &Container::addProvider, shared_from_this(), std::placeholders::_2
    );
    _collectionItemRemovedSlots[collection] = collection->itemRemoved() += std::bind(
        &Container::removeProvider, shared_from_this(), std::placeholders::_2
    );

    addProvider(collection->_lengthProvider);
    for (auto provider : collection->items())
        doAddProvider(provider, collection);

    _collectionAdded->execute(shared_from_this(), collection);
}

void
Container::removeCollection(std::shared_ptr<Collection> collection)
{
    _collections.remove(collection);

    _collectionItemAddedSlots.erase(collection);
    _collectionItemRemovedSlots.erase(collection);

    removeProvider(collection->_lengthProvider);
    for (auto provider : collection->items())
        doRemoveProvider(provider, collection);

    _collectionRemoved->execute(shared_from_this(), collection);
}

void
Container::providerPropertyChangedHandler(Provider::Ptr         provider,
                                          Collection::Ptr       collection,
                                          const std::string&    propertyName)
{
    auto formattedPropertyName = propertyName;

    // provider is in a collection
    if (collection)
        formattedPropertyName = formatPropertyName(collection, provider, propertyName);
    
    if (_propertyChanged.count(formattedPropertyName) != 0)
        propertyChanged(formattedPropertyName)->execute(shared_from_this(), propertyName, formattedPropertyName);
}

void
Container::providerPropertyAddedHandler(Provider::Ptr       provider,
                                        Collection::Ptr     collection,
                                        const std::string& 	propertyName)
{
    _propertyAdded->execute(shared_from_this(), propertyName, formatPropertyName(collection, provider, propertyName));
    providerPropertyChangedHandler(provider, collection, propertyName);
}


void
Container::providerPropertyRemovedHandler(Provider::Ptr         provider,
                                          Collection::Ptr       collection,
                                          const std::string&	propertyName)
{
    if (_propertyChanged.count(propertyName) && _propertyChanged[propertyName]->numCallbacks() == 0)
        _propertyChanged.erase(propertyName);

    _propertyRemoved->execute(
        shared_from_this(),
        propertyName,
        formatPropertyName(collection, provider, propertyName)
    );
}

std::pair<Provider::Ptr, std::string>
Container::getProviderByPropertyName(const std::string& propertyName) const
{
    auto pos = propertyName.find_first_of("[");

    if (pos != std::string::npos)
    {
        auto collectionName = propertyName.substr(0, pos);

        for (auto collection : _collections)
            if (collection->name() == collectionName)
            {
                auto pos2 = propertyName.find_first_of("]");
                auto indexStr = propertyName.substr(pos + 1, pos2 - pos - 1);

                return std::pair<Provider::Ptr, std::string>(
                    collection->items()[std::stoi(indexStr)],
                    propertyName.substr(pos2 + 2)
                );
            }
    }
    else
    {
        for (auto provider : _providers)
            if (provider->hasProperty(propertyName))
                return std::pair<Provider::Ptr, std::string>(provider, propertyName);
    }

    return std::pair<Provider::Ptr, std::string>(nullptr, propertyName);
}

bool
Container::hasProperty(const std::string& propertyName) const
{
    return std::get<0>(getProviderByPropertyName(propertyName)) != nullptr;
}

void
Container::doAddProvider(ProviderPtr provider, CollectionPtr collection)
{
    assert(std::find(_providers.begin(), _providers.end(), provider) == _providers.end());

    _providers.push_back(provider);

    _propertySlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
        &Container::providerPropertyAddedHandler,
        shared_from_this(),
        provider,
        collection,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
        &Container::providerPropertyRemovedHandler,
        shared_from_this(),
        provider,
        collection,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyChanged()->connect(std::bind(
        &Container::providerPropertyChangedHandler,
        shared_from_this(),
        provider,
        collection,
        std::placeholders::_2
    )));

    for (auto property : provider->values())
        providerPropertyAddedHandler(provider, collection, property.first);

    _providerAdded->execute(shared_from_this(), provider);
}

void
Container::doRemoveProvider(ProviderPtr provider, CollectionPtr collection)
{
    assert(std::find(_providers.begin(), _providers.end(), provider) != _providers.end());

    // we have to make sure the provider is not 
    for (auto property : provider->values())
        providerPropertyRemovedHandler(provider, nullptr, property.first);

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

std::string
Container::formatPropertyName(Collection::Ptr collection, Provider::Ptr provider, const std::string& propertyName)
{
    if (collection == nullptr)
        return propertyName;

    auto it = std::find(collection->items().begin(), collection->items().end(), provider);

    return formatPropertyName(collection, it - collection->items().begin(), propertyName);
}

std::string
Container::formatPropertyName(Collection::Ptr collection, uint index, const std::string& propertyName)
{
    if (collection == nullptr)
        return propertyName;

    return collection->name() + "[" + std::to_string(index) + "]." + propertyName;
}
