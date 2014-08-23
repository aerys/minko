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
    _lengthProvider(nullptr)
{
}

void
Container::addCollection(std::shared_ptr<Collection> collection)
{
    _collections.push_back(collection);

    _collectionItemAddedSlots[collection] = collection->itemAdded() += std::bind(
        &Container::doAddProvider, this, std::placeholders::_2, collection
    );
    _collectionItemRemovedSlots[collection] = collection->itemRemoved() += std::bind(
        &Container::doRemoveProvider, this, std::placeholders::_2, collection
    );

    if (collection->items().size() != 0)
        for (auto provider : collection->items())
            doAddProvider(provider, collection);
    else
        updateCollectionLength(collection);
}

void
Container::removeCollection(std::shared_ptr<Collection> collection)
{
    _collections.remove(collection);

    _collectionItemAddedSlots.erase(collection);
    _collectionItemRemovedSlots.erase(collection);

    for (auto provider : collection->items())
        doRemoveProvider(provider, collection);
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
    
    _propertyChanged.execute(*this, propertyName, formattedPropertyName);
    if (_propertyNameToChangedSignal.count(formattedPropertyName) != 0)
        propertyChanged(formattedPropertyName).execute(*this, propertyName, formattedPropertyName);
}

void
Container::providerPropertyAddedHandler(Provider::Ptr       provider,
                                        Collection::Ptr     collection,
                                        const std::string& 	propertyName)
{
    _propertyAdded.execute(*this, propertyName, formatPropertyName(collection, provider, propertyName));
    providerPropertyChangedHandler(provider, collection, propertyName);
}


void
Container::providerPropertyRemovedHandler(Provider::Ptr         provider,
                                          Collection::Ptr       collection,
                                          const std::string&	propertyName)
{
    auto formattedPropertyName = propertyName;

    if (collection)
        formattedPropertyName = formatPropertyName(collection, provider, propertyName);

    _propertyChanged.execute(*this, propertyName, formattedPropertyName);
    if (_propertyNameToChangedSignal.count(formattedPropertyName) != 0)
        propertyChanged(formattedPropertyName).execute(*this, propertyName, formattedPropertyName);

    _propertyRemoved.execute(*this, propertyName, formattedPropertyName);

    if (_propertyNameToChangedSignal.count(propertyName)
        && _propertyNameToChangedSignal[propertyName].numCallbacks() == 0)
        _propertyNameToChangedSignal.erase(propertyName);
}

std::pair<Provider::Ptr, std::string>
Container::getProviderByPropertyName(const std::string& propertyName) const
{
    auto pos = propertyName.find_first_of("[");

    if (pos != std::string::npos)
    {
        auto collectionName = propertyName.substr(0, pos);

        for (const auto& collection : _collections)
            if (collection->name() == collectionName)
            {
                auto pos2 = propertyName.find_first_of("]");
                auto index = (uint)std::stoi(propertyName.substr(pos + 1, pos2 - pos - 1));
                auto token = propertyName.substr(pos2 + 2);

                if (index < collection->items().size())
                {
                    auto provider = collection->items()[index];

                    if (provider->hasProperty(token) != 0)
                        return std::pair<Provider::Ptr, std::string>(provider, token);
                }

                return std::pair<Provider::Ptr, std::string>(nullptr, token);
            }
    }
    else
    {
        for (const auto& provider : _providers)
            if (provider->hasProperty(propertyName) != 0)
                return std::pair<Provider::Ptr, std::string>(provider, propertyName);
    }

    return std::pair<Provider::Ptr, std::string>(nullptr, propertyName);
}

void
Container::doAddProvider(ProviderPtr provider, CollectionPtr collection)
{
    _providers.push_back(provider);

    _propertySlots[provider].push_back(provider->propertyAdded().connect(std::bind(
        &Container::providerPropertyAddedHandler,
        this,
        provider,
        collection,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyRemoved().connect(std::bind(
        &Container::providerPropertyRemovedHandler,
        this,
        provider,
        collection,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyChanged().connect(std::bind(
        &Container::providerPropertyChangedHandler,
        this,
        provider,
        collection,
        std::placeholders::_2
    )));

    for (auto property : provider->values())
        providerPropertyAddedHandler(provider, collection, property.first);

    if (collection)
        updateCollectionLength(collection);
}

void
Container::updateCollectionLength(data::Collection::Ptr collection)
{
    if (_lengthProvider == nullptr)
    {
        _lengthProvider = Provider::create();
        doAddProvider(_lengthProvider);
    }

    _lengthProvider->set(collection->name() + ".length", collection->items().size());
}

void
Container::doRemoveProvider(ProviderPtr provider, CollectionPtr collection)
{
    assert(std::find(_providers.begin(), _providers.end(), provider) != _providers.end());

    // execute all the "property removed" signals
    for (auto property : provider->values())
        providerPropertyRemovedHandler(provider, nullptr, property.first);

    _providers.erase(std::find(_providers.begin(), _providers.end(), provider));

    // erase all the slots (property added, changed, removed) for this provider
    _propertySlots.erase(provider);

    // destroy all signals that might have been created for each property declared by the provider
    // warning! erase the signal only if it has no callbacks anymore, otherwise it should be kept valid
    if (!collection)
    {
        for (const auto& nameAndValue : provider->values())
            if (_propertyNameToChangedSignal.count(nameAndValue.first) != 0
                && _propertyNameToChangedSignal[nameAndValue.first].numCallbacks() == 0)
                _propertyNameToChangedSignal.erase(nameAndValue.first);
    }
    else
    {
        int providerIndex = std::find(collection->items().begin(), collection->items().end(), provider)
            - collection->items().begin();
        auto prefix = collection->name() + "[" + std::to_string(providerIndex) + "].";
        
        for (const auto& nameAndValue : provider->values())
            if (_propertyNameToChangedSignal.count(prefix + nameAndValue.first) != 0
                && _propertyNameToChangedSignal[prefix + nameAndValue.first].numCallbacks() == 0)
                _propertyNameToChangedSignal.erase(prefix + nameAndValue.first);

        updateCollectionLength(collection);
    }

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

void
Container::addProviderToCollection(std::shared_ptr<data::Provider> provider,
                                   const std::string&              collectionName)
{
    auto collectionIt = std::find_if(_collections.begin(), _collections.end(), [&](data::Collection::Ptr c)
    {
        return c->name() == collectionName;
    });

    data::Collection::Ptr collection;

    // if the collection does not already exist
    if (collectionIt == _collections.end())
    {
        // create and add it
        collection = data::Collection::create(collectionName);
        addCollection(collection);
    }
    else
    {
        // just use the existing collection
        collection = *collectionIt;
    }

    collection->pushBack(provider);
}

void
Container::removeProviderFromCollection(std::shared_ptr<data::Provider> provider,
                                        const std::string&              collectionName)
{
    auto collectionIt = std::find_if(_collections.begin(), _collections.end(), [&](data::Collection::Ptr c)
    {
        return c->name() == collectionName;
    });

    if (collectionIt == _collections.end())
        throw std::invalid_argument("collectionName = " + collectionName);

    (*collectionIt)->remove(provider);
}


const std::string
Container::getActualPropertyName(const std::unordered_map<std::string, std::string>&    vars,
                                 const std::string&                                     propertyName)
{
    for (const auto& variableName : vars)
    {
        auto pos = propertyName.find("${" + variableName.first + "}");

        if (pos != std::string::npos)
            return propertyName.substr(0, pos) + variableName.second
                + propertyName.substr(pos + variableName.first.size() + 3);
    }

    return propertyName;
}

