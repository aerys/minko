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

#include "sparsehash/sparse_hash_map"

using namespace minko;
using namespace minko::data;

Store::Store() :
    _providers(),
    _collections(),
    _lengthProvider(nullptr),
    _propertyAdded(),
    _propertyRemoved(),
    _propertyChanged(),
    _propertyNameToChangedSignal(new ChangedSignalMap()),
    _propertyNameToAddedSignal(new ChangedSignalMap()),
    _propertyNameToRemovedSignal(new ChangedSignalMap()),
    _propertySlots(new ProviderToChangedSlotListMap()),
    _collectionItemAddedSlots(new CollectionToChangedSlotMap()),
    _collectionItemRemovedSlots(new CollectionToChangedSlotMap())
{
    initialize();
}

Store::Store(const Store& store) :
    _providers(),
    _collections(),
    _lengthProvider(nullptr),
    _propertyAdded(),
    _propertyRemoved(),
    _propertyChanged(),
    _propertyNameToChangedSignal(new ChangedSignalMap()),
    _propertyNameToAddedSignal(new ChangedSignalMap()),
    _propertyNameToRemovedSignal(new ChangedSignalMap()),
    _propertySlots(new ProviderToChangedSlotListMap()),
    _collectionItemAddedSlots(new CollectionToChangedSlotMap()),
    _collectionItemRemovedSlots(new CollectionToChangedSlotMap())
{
    initialize();
    copyFrom(store, true);
}

Store::Store(const Store& store, bool deepCopy) :
    _providers(),
    _collections(),
    _lengthProvider(nullptr),
    _propertyAdded(),
    _propertyRemoved(),
    _propertyChanged(),
    _propertyNameToChangedSignal(new ChangedSignalMap()),
    _propertyNameToAddedSignal(new ChangedSignalMap()),
    _propertyNameToRemovedSignal(new ChangedSignalMap()),
    _propertySlots(new ProviderToChangedSlotListMap()),
    _collectionItemAddedSlots(new CollectionToChangedSlotMap()),
    _collectionItemRemovedSlots(new CollectionToChangedSlotMap())
{
    initialize();
    copyFrom(store, deepCopy);
}

Store::Store(Store&& other)
{
    _providers = std::move(other._providers);
    _collections = std::move(other._collections);
    _lengthProvider = std::move(other._lengthProvider);
    _propertyAdded = std::move(other._propertyAdded);
    _propertyRemoved = std::move(other._propertyRemoved);
    _propertyChanged = std::move(other._propertyChanged);
    _propertyNameToChangedSignal = std::move(other._propertyNameToChangedSignal);
    _propertyNameToAddedSignal = std::move(other._propertyNameToAddedSignal);
    _propertyNameToRemovedSignal = std::move(other._propertyNameToRemovedSignal);
    _propertySlots = std::move(other._propertySlots);
    _collectionItemAddedSlots = std::move(other._collectionItemAddedSlots);
    _collectionItemRemovedSlots = std::move(other._collectionItemRemovedSlots);

    other._propertyNameToChangedSignal = nullptr;
    other._propertyNameToAddedSignal = nullptr;
    other._propertyNameToRemovedSignal = nullptr;
    other._propertySlots = nullptr;
    other._collectionItemAddedSlots = nullptr;
    other._collectionItemRemovedSlots = nullptr;
}

Store&
Store::operator=(Store&& other)
{
    _providers = std::move(other._providers);
    _collections = std::move(other._collections);
    _lengthProvider = std::move(other._lengthProvider);
    _propertyAdded = std::move(other._propertyAdded);
    _propertyRemoved = std::move(other._propertyRemoved);
    _propertyChanged = std::move(other._propertyChanged);
    _propertyNameToChangedSignal = other._propertyNameToChangedSignal;
    _propertyNameToAddedSignal = other._propertyNameToAddedSignal;
    _propertyNameToRemovedSignal = other._propertyNameToRemovedSignal;
    _propertySlots = other._propertySlots;
    _collectionItemAddedSlots = other._collectionItemAddedSlots;
    _collectionItemRemovedSlots = other._collectionItemRemovedSlots;

    other._propertyNameToChangedSignal = nullptr;
    other._propertyNameToAddedSignal = nullptr;
    other._propertyNameToRemovedSignal = nullptr;
    other._propertySlots = nullptr;
    other._collectionItemAddedSlots = nullptr;
    other._collectionItemRemovedSlots = nullptr;

    return *this;
}

void
Store::initialize()
{
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _propertyNameToChangedSignal->set_deleted_key("");
    _propertyNameToAddedSignal->set_deleted_key("");
    _propertyNameToRemovedSignal->set_deleted_key("");
    _propertySlots->set_deleted_key(nullptr);
    _collectionItemAddedSlots->set_deleted_key(nullptr);
    _collectionItemRemovedSlots->set_deleted_key(nullptr);
#endif
}

void
Store::copyFrom(const Store& store, bool deepCopy)
{
    if (deepCopy)
    {
        std::list<data::Provider::Ptr> added;

        for (auto collection : store._collections)
        {
            added.insert(added.end(), collection->items().begin(), collection->items().end());
            addCollection(Collection::create(collection));
        }

        for (auto provider : store._providers)
        {
            auto it = std::find(added.begin(), added.end(), provider);

            if (it == added.end())
                _providers.push_back(Provider::create(provider));
        }
    }
    else
    {
        _collections = store._collections;
        _providers = store._providers;
        if (store._lengthProvider)
            _lengthProvider = Provider::create(store._lengthProvider);
    }
}

Store::~Store()
{
    if (_propertyNameToChangedSignal)
        for (auto& it : *_propertyNameToChangedSignal)
            delete it.second;
    delete _propertyNameToChangedSignal;

    if (_propertyNameToAddedSignal)
        for (auto& it : *_propertyNameToAddedSignal)
            delete it.second;
    delete _propertyNameToAddedSignal;

    if (_propertyNameToRemovedSignal)
        for (auto& it : *_propertyNameToRemovedSignal)
            delete it.second;
    delete _propertyNameToRemovedSignal;

    delete _propertySlots;
    delete _collectionItemAddedSlots;
    delete _collectionItemRemovedSlots;
}

Store::PropertyChangedSignal&
Store::getOrInsertSignal(ChangedSignalMap* signals, const PropertyName& propertyName)
{
    PropertyChangedSignal* signal;
    if (signals->count(propertyName) == 0)
    {
        signal = new PropertyChangedSignal();
        signals->insert(std::make_pair(propertyName, signal));
    }
    else
        signal = signals->find(propertyName)->second;

    return *signal;
}

bool
Store::hasPropertyAddedSignal(const PropertyName& propertyName) const
{
    return _propertyNameToAddedSignal->count(propertyName) != 0;
}

bool
Store::hasPropertyRemovedSignal(const PropertyName& propertyName) const
{
    return _propertyNameToRemovedSignal->count(propertyName) != 0;
}

bool
Store::hasPropertyChangedSignal(const PropertyName& propertyName) const
{
    return _propertyNameToChangedSignal->count(propertyName) != 0;
}

void
Store::addCollection(std::shared_ptr<Collection> collection)
{
    _collections.push_back(collection);

    (*_collectionItemAddedSlots)[collection] = collection->itemAdded().connect(
        [this, collection](Collection&, Provider::Ptr provider)
        {
            doAddProvider(provider, collection);
        }
    );
    (*_collectionItemRemovedSlots)[collection] = collection->itemRemoved().connect(
        [this, collection](Collection&, Provider::Ptr provider)
        {
            doRemoveProvider(provider, collection);
        }
    );

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

    _collectionItemAddedSlots->erase(collection);
    _collectionItemRemovedSlots->erase(collection);

    for (auto provider : collection->items())
        doRemoveProvider(provider, collection);
}

void
Store::executePropertySignal(Provider::Ptr                  provider,
                             Collection::Ptr                collection,
                             const PropertyName&            propertyName,
                             const PropertyChangedSignal&   anyChangedSignal,
                             const ChangedSignalMap&        propertyNameToSignal)
{
    anyChangedSignal.execute(*this, provider, propertyName);
    if (collection)
    {
        auto formattedPropertyName = formatPropertyName(collection, provider, *propertyName, true);
        if (propertyNameToSignal.count(formattedPropertyName) != 0)
            propertyNameToSignal.find(formattedPropertyName)->second->execute(*this, provider, propertyName);

        formattedPropertyName = formatPropertyName(collection, provider, *propertyName);
        if (propertyNameToSignal.count(formattedPropertyName) != 0)
            propertyNameToSignal.find(formattedPropertyName)->second->execute(*this, provider, propertyName);
    }
    else if (propertyNameToSignal.count(propertyName) != 0)
        propertyNameToSignal.find(propertyName)->second->execute(*this, provider, propertyName);
}

void
Store::providerPropertyAddedHandler(Provider::Ptr                   provider,
                                    Collection::Ptr                 collection,
                                    const Provider::PropertyName& 	propertyName)
{
    executePropertySignal(provider, collection, propertyName, _propertyAdded, *_propertyNameToAddedSignal);
    executePropertySignal(provider, collection, propertyName, _propertyChanged, *_propertyNameToChangedSignal);
}

void
Store::providerPropertyRemovedHandler(Provider::Ptr                 provider,
                                      Collection::Ptr               collection,
                                      const Provider::PropertyName&	propertyName)
{
    executePropertySignal(provider, collection, propertyName, _propertyChanged, *_propertyNameToChangedSignal);
    executePropertySignal(provider, collection, propertyName, _propertyRemoved, *_propertyNameToRemovedSignal);

    auto formattedName = formatPropertyName(collection, provider, *propertyName);

    // Explicit typing required here since `map::erase` takes a `const_iterator`
    // while find returns an `iterator` by default.
#ifdef MINKO_USE_SPARSE_HASH_MAP
    ChangedSignalMap::iterator it;
#else
    ChangedSignalMap::const_iterator it;
#endif

    it = _propertyNameToAddedSignal->find(formattedName);
    if (it != _propertyNameToAddedSignal->end() && it->second->numCallbacks() == 0)
    {
        delete it->second;
        _propertyNameToAddedSignal->erase(it);
    }
    it = _propertyNameToRemovedSignal->find(formattedName);
    if (it != _propertyNameToRemovedSignal->end() && it->second->numCallbacks() == 0)
    {
        delete it->second;
        _propertyNameToRemovedSignal->erase(it);
    }
    it = _propertyNameToChangedSignal->find(formattedName);
    if (it != _propertyNameToChangedSignal->end() && it->second->numCallbacks() == 0)
    {
        delete it->second;
        _propertyNameToChangedSignal->erase(it);
    }

    formattedName = formatPropertyName(collection, provider, *propertyName, true);
    it = _propertyNameToAddedSignal->find(formattedName);
    if (it != _propertyNameToAddedSignal->end() && it->second->numCallbacks() == 0)
    {
        delete it->second;
        _propertyNameToAddedSignal->erase(it);
    }
    it = _propertyNameToRemovedSignal->find(formattedName);
    if (it != _propertyNameToRemovedSignal->end() && it->second->numCallbacks() == 0)
    {
        delete it->second;
        _propertyNameToRemovedSignal->erase(it);
    }
    it = _propertyNameToChangedSignal->find(formattedName);
    if (it != _propertyNameToChangedSignal->end() && it->second->numCallbacks() == 0)
    {
        delete it->second;
        _propertyNameToChangedSignal->erase(it);
    }
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
        {
            if (provider->hasProperty(propertyName))
                return std::pair<Provider::Ptr, std::string>(provider, propertyName);
        }
    }

    return std::pair<Provider::Ptr, std::string>(nullptr, propertyName);
}

void
Store::doAddProvider(ProviderPtr provider, CollectionPtr collection)
{
    _providers.push_back(provider);

    (*_propertySlots)[provider].push_back(provider->propertyAdded().connect(
        [this, collection](Provider::Ptr p, const Provider::PropertyName& propertyName)
        {
            providerPropertyAddedHandler(p, collection, propertyName);
        }
    ));

    (*_propertySlots)[provider].push_back(provider->propertyRemoved().connect(
        [this, collection](Provider::Ptr p, const Provider::PropertyName& propertyName)
        {
            providerPropertyRemovedHandler(p, collection, propertyName);
        }
    ));

    (*_propertySlots)[provider].push_back(provider->propertyChanged().connect(
        [this, collection](Provider::Ptr p, const PropertyName& propertyName)
        {
            executePropertySignal(p, collection, propertyName, _propertyChanged, *_propertyNameToChangedSignal);
        }
    ));

    for (const auto& property : provider->values())
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

    _lengthProvider->set<int>(*collection->name() + ".length", collection->items().size());
}

void
Store::doRemoveProvider(ProviderPtr provider, CollectionPtr collection)
{
    auto it = std::find(_providers.begin(), _providers.end(), provider);

    assert(provider != nullptr);
    assert(it != _providers.end());

    _providers.erase(it);
    //if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
        //return;

    // execute all the "property removed" signals
    for (const auto& property : provider->values())
        providerPropertyRemovedHandler(provider, collection, property.first);

    // erase all the slots (property added, changed, removed) for this provider
    _propertySlots->erase(provider);

    // destroy all signals that might have been created for each property declared by the provider
    // warning! erase the signal only if it has no callbacks anymore, otherwise it should be kept valid
    if (!collection)
    {
        for (const auto& nameAndValue : provider->values())
            if (_propertyNameToChangedSignal->count(nameAndValue.first) != 0
                && (*_propertyNameToChangedSignal)[nameAndValue.first]->numCallbacks() == 0)
                _propertyNameToChangedSignal->erase(nameAndValue.first);
    }
    else
    {
        int providerIndex = std::find(collection->items().begin(), collection->items().end(), provider)
            - collection->items().begin();
        auto prefix = *collection->name() + "[" + std::to_string(providerIndex) + "].";

        for (const auto& nameAndValue : provider->values())
            if (_propertyNameToChangedSignal->count(prefix + *nameAndValue.first) != 0
                && (*_propertyNameToChangedSignal)[prefix + *nameAndValue.first]->numCallbacks() == 0)
                _propertyNameToChangedSignal->erase(prefix + *nameAndValue.first);

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
                executePropertySignal(provider, collection, property.first, _propertyChanged, *_propertyNameToChangedSignal);
        }
    }

}

Store::PropertyName
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

Store::PropertyName
Store::formatPropertyName(Collection::Ptr collection, const std::string& index, const std::string& propertyName)
{
    if (collection == nullptr)
        return propertyName;

    return PropertyName(*collection->name() + "[" + index + "]." + propertyName);
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
Store::getActualPropertyName(const FStringList& vars, const FString& propertyName)
{
    std::string s = *propertyName;

    // FIXME: order vars keys from longer to shorter in order to match the longest matching var name
    // or use regex_replace

    for (const auto& variableName : vars)
    {
        auto pos = (*propertyName).find("${" + *variableName.first + "}");

        if (pos != std::string::npos)
            s = s.substr(0, pos) + *variableName.second + s.substr(pos + (*variableName.first).size() + 3);
        else if ((pos = (*propertyName).find("$" + *variableName.first)) != std::string::npos)
            s = s.substr(0, pos) + *variableName.second + s.substr(pos + (*variableName.first).size() + 1);
    }

    return s;
}
