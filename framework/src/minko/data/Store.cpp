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

#include "minko/data/Store.hpp"

#include "minko/data/Provider.hpp"
#include "minko/data/Collection.hpp"
#include "minko/data/AbstractFilter.hpp"

using namespace minko;
using namespace minko::data;

Store::Store() :
    _lengthProvider(nullptr)
{
}

void
Store::addCollection(std::shared_ptr<Collection> collection)
{
    _collections.push_back(collection);

    _collectionItemAddedSlots[collection] = collection->itemAdded().connect(std::bind(
        &Store::doAddProvider, this, std::placeholders::_2, collection
    ));
    _collectionItemRemovedSlots[collection] = collection->itemRemoved().connect(std::bind(
        &Store::doRemoveProvider, this, std::placeholders::_2, collection
    ));

    if (collection->items().size() != 0)
        for (auto provider : collection->items())
            doAddProvider(provider, collection);
    else
        updateCollectionLength(collection);
}

void
Store::removeCollection(std::shared_ptr<Collection> collection)
{
    _collections.remove(collection);

    _collectionItemAddedSlots.erase(collection);
    _collectionItemRemovedSlots.erase(collection);

    for (auto provider : collection->items())
        doRemoveProvider(provider, collection);
}

void
Store::executePropertySignal(Provider::Ptr                                        provider,
                                 Collection::Ptr                                      collection,
                                 const std::string&                                   propertyName,
                                 const PropertyChangedSignal&                         anyChangedSignal,
                                 const std::map<std::string, PropertyChangedSignal>&  propertyNameToSignal)
{
    anyChangedSignal.execute(*this, provider, propertyName);
    if (collection)
    {
        auto formattedPropertyName = formatPropertyName(collection, provider, propertyName, true);
        if (propertyNameToSignal.count(formattedPropertyName) != 0)
            propertyNameToSignal.at(formattedPropertyName).execute(*this, provider, propertyName);

        formattedPropertyName = formatPropertyName(collection, provider, propertyName);
        if (propertyNameToSignal.count(formattedPropertyName) != 0)
            propertyNameToSignal.at(formattedPropertyName).execute(*this, provider, propertyName);
    }
    else if (propertyNameToSignal.count(propertyName) != 0)
        propertyNameToSignal.at(propertyName).execute(*this, provider, propertyName);
}

void
Store::providerPropertyAddedHandler(Provider::Ptr       provider,
                                        Collection::Ptr     collection,
                                        const std::string& 	propertyName)
{
    executePropertySignal(provider, collection, propertyName, _propertyAdded, _propertyNameToAddedSignal);
    executePropertySignal(provider, collection, propertyName, _propertyChanged, _propertyNameToChangedSignal);
}


void
Store::providerPropertyRemovedHandler(Provider::Ptr         provider,
                                          Collection::Ptr       collection,
                                          const std::string&	propertyName)
{
    executePropertySignal(provider, collection, propertyName, _propertyChanged, _propertyNameToChangedSignal);
    executePropertySignal(provider, collection, propertyName, _propertyRemoved, _propertyNameToRemovedSignal);

    auto formattedName = formatPropertyName(collection, provider, propertyName);
    if (_propertyNameToAddedSignal.count(formattedName)
        && _propertyNameToAddedSignal[formattedName].numCallbacks() == 0)
        _propertyNameToAddedSignal.erase(formattedName);
    if (_propertyNameToRemovedSignal.count(formattedName)
        && _propertyNameToRemovedSignal[formattedName].numCallbacks() == 0)
        _propertyNameToRemovedSignal.erase(formattedName);
    if (_propertyNameToChangedSignal.count(formattedName)
        && _propertyNameToChangedSignal[formattedName].numCallbacks() == 0)
        _propertyNameToChangedSignal.erase(formattedName);

    formattedName = formatPropertyName(collection, provider, propertyName, true);
    if (_propertyNameToAddedSignal.count(formattedName)
        && _propertyNameToAddedSignal[formattedName].numCallbacks() == 0)
        _propertyNameToAddedSignal.erase(formattedName);
    if (_propertyNameToRemovedSignal.count(formattedName)
        && _propertyNameToRemovedSignal[formattedName].numCallbacks() == 0)
        _propertyNameToRemovedSignal.erase(formattedName);
    if (_propertyNameToChangedSignal.count(formattedName)
        && _propertyNameToChangedSignal[formattedName].numCallbacks() == 0)
        _propertyNameToChangedSignal.erase(formattedName);
}

std::pair<Provider::Ptr, std::string>
Store::getProviderByPropertyName(const std::string& propertyName) const
{
    auto pos = propertyName.find_first_of("[");

    if (pos != std::string::npos)
    {
        auto collectionName = propertyName.substr(0, pos);

        for (const auto& collection : _collections)
            if (collection->name() == collectionName)
            {
                auto pos2 = propertyName.find_first_of("]");
                auto indexStr = propertyName.substr(pos + 1, pos2 - pos - 1);
                auto pos3 = indexStr.find_first_of("-");
                auto token = propertyName.substr(pos2 + 2);

                // fetch provider by uuid
                if (pos3 != std::string::npos && pos3 < pos2)
                {
                    for (const auto& provider : collection->items())
                        if (provider->uuid() == indexStr && provider->hasProperty(token))
                            return std::pair<Provider::Ptr, std::string>(provider, token);
                }
                else // fetch provider by index
                {
                    uint index = std::atoi(indexStr.c_str());

                    if (index < collection->items().size())
                    {
                        auto provider = collection->items()[index];

                        if (provider->hasProperty(token))
                            return std::pair<Provider::Ptr, std::string>(provider, token);
                    }
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
Store::doAddProvider(ProviderPtr provider, CollectionPtr collection)
{
    _providers.push_back(provider);

    _propertySlots[provider].push_back(provider->propertyAdded().connect(std::bind(
        &Store::providerPropertyAddedHandler,
        this,
        provider,
        collection,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyRemoved().connect(std::bind(
        &Store::providerPropertyRemovedHandler,
        this,
        provider,
        collection,
        std::placeholders::_2
    )));

    _propertySlots[provider].push_back(provider->propertyChanged().connect(std::bind(
        &Store::executePropertySignal,
        this,
        provider,
        collection,
        std::placeholders::_2,
        std::ref(_propertyChanged),
        std::ref(_propertyNameToChangedSignal)
    )));

    for (auto property : provider->values())
        providerPropertyAddedHandler(provider, collection, property.first);

    if (collection)
        updateCollectionLength(collection);
}

void
Store::updateCollectionLength(data::Collection::Ptr collection)
{
    if (_lengthProvider == nullptr)
    {
        _lengthProvider = Provider::create();
        doAddProvider(_lengthProvider);
    }

    _lengthProvider->set(collection->name() + ".length", collection->items().size());
}

void
Store::doRemoveProvider(ProviderPtr provider, CollectionPtr collection)
{
    assert(std::find(_providers.begin(), _providers.end(), provider) != _providers.end());

    // execute all the "property removed" signals
    for (auto property : provider->values())
        providerPropertyRemovedHandler(provider, collection, property.first);

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

        // the removed provider might very well be anything but the last item of the collection
        // thus, all properties of all providers will have a different name.
        // Ex: "material[2].diffuseMap" will become "material[1].diffuseMap" when the material 1
        // is removed.
        // In other words, the value targeted by "material[1].diffuseMap" will be different and thus
        // we should trigger the "property changed" signal for each property of each provider which is
        // "after" the one being removed from the collection.
        for (uint i = providerIndex; i < collection->items().size(); ++i)
        {
            const auto& provider = collection->items()[i];

            for (const auto& property : provider->values())
                executePropertySignal(provider, collection, property.first, _propertyChanged, _propertyNameToChangedSignal);
        }
    }

}

std::string
Store::formatPropertyName(Collection::Ptr       collection,
                          Provider::Ptr         provider,
                          const std::string&    propertyName,
                          bool                  useUuid)
{
    if (collection == nullptr)
        return propertyName;

    if (useUuid)
        return formatPropertyName(collection, provider->uuid(), propertyName);

    auto it = std::find(collection->items().begin(), collection->items().end(), provider);

    return formatPropertyName(collection, std::to_string(it - collection->items().begin()), propertyName);
}

std::string
Store::formatPropertyName(Collection::Ptr collection, const std::string& index, const std::string& propertyName)
{
    if (collection == nullptr)
        return propertyName;

    return collection->name() + "[" + index + "]." + propertyName;
}

void
Store::addProviderToCollection(std::shared_ptr<data::Provider> provider,
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
Store::removeProviderFromCollection(std::shared_ptr<data::Provider> provider,
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
Store::getActualPropertyName(const std::unordered_map<std::string, std::string>&    vars,
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

