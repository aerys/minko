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

#include "minko/component/AbstractComponent.hpp"

#include "minko/scene/Node.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/Collection.hpp"

using namespace minko;
using namespace minko::component;

void
AbstractComponent::addProviderToCollection(std::shared_ptr<data::Provider> provider,
                                           const std::string&              collectionName)
{
    assert(target() != nullptr);

    const auto& collections = target()->data()->collections();
    auto collectionIt = std::find_if(collections.begin(), collections.end(), [&](data::Collection::Ptr c)
    {
        return c->name() == collectionName;
    });

    data::Collection::Ptr collection;

    // if the collection does not already exist
    if (collectionIt == collections.end())
    {
        // create and add it
        collection = data::Collection::create(collectionName);
        target()->data()->addCollection(collection);
    }
    else
    {
        // just use the existing collection
        collection = *collectionIt;
    }

    collection->pushBack(provider);
}

void
AbstractComponent::removeProviderFromCollection(std::shared_ptr<data::Provider> provider,
                                                const std::string&              collectionName)
{
    assert(target() != nullptr);

    const auto& collections = target()->data()->collections();
    auto collectionIt = std::find_if(collections.begin(), collections.end(), [&](data::Collection::Ptr c)
    {
        return c->name() == collectionName;
    });

    if (collectionIt == collections.end())
        throw std::invalid_argument("collectionName = " + collectionName);

    (*collectionIt)->remove(provider);
}
