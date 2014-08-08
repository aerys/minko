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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
    namespace data
    {
        class Collection
        {
            friend class Container;

        public:
            typedef std::shared_ptr<Collection>                     Ptr;

        private:
            typedef std::shared_ptr<Provider>                       ProviderPtr;
            typedef std::vector<ProviderPtr>                        Items;
            typedef Signal<ProviderPtr, const std::string&>::Ptr    PropertySignalPtr;
            typedef Signal<ProviderPtr, const std::string&>::Slot   PropertySignalSlot;

        private:
            std::string                         _name;
            Items                               _items;
            ProviderPtr                         _lengthProvider;

            Signal<Collection&, ProviderPtr>    _itemAdded;
            Signal<Collection&, ProviderPtr>    _itemRemoved;

        public:
            static inline
            Ptr
            create(const std::string& name)
            {
                return std::shared_ptr<Collection>(new Collection(name));
            }

            inline
            const std::string&
            name()
            {
                return _name;
            }

            inline
            const Items&
            items()
            {
                return _items;
            }

            inline
            Signal<Collection&, ProviderPtr>&
            itemAdded()
            {
                return _itemAdded;
            }

            inline
            Signal<Collection&, ProviderPtr>&
            itemRemoved()
            {
                return _itemRemoved;
            }

            inline
            ProviderPtr
            front()
            {
                return _items.front();
            }

            inline
            ProviderPtr
            back()
            {
                return _items.back();
            }

            inline
            Items::const_iterator
            begin()
            {
                return _items.begin();
            }

            inline
            Items::const_iterator
            end()
            {
                return _items.end();
            }

            inline
            Collection&
            insert(Items::const_iterator position, ProviderPtr provider)
            {
                _items.insert(position, provider);
                addProvider(provider);
                
                return *this;
            }

            inline
            Collection&
            erase(Items::const_iterator position)
            {
                auto provider = *position;

                _items.erase(position);
                removeProvider(provider);

                return *this;
            }

            inline
            Collection&
            remove(ProviderPtr provider)
            {
                _items.erase(std::find(_items.begin(), _items.end(), provider));
                removeProvider(provider);

                return *this;
            }

            inline
            Collection&
            pushBack(ProviderPtr provider)
            {
                _items.push_back(provider);
                addProvider(provider);

                return *this;
            }

            inline
            Collection&
            popBack()
            {
                auto provider = _items.back();

                _items.pop_back();
                removeProvider(provider);

                return *this;
            }

        private:
            Collection(const std::string& name) :
                _name(name),
                _lengthProvider(Provider::create())
            {
                _lengthProvider->set("length", 0u);
            }

            void
            addProvider(ProviderPtr provider)
            {
                //_lengthProvider->set("length", _items.size());
                _itemAdded.execute(*this, provider);
            }

            void
            removeProvider(ProviderPtr provider)
            {
                //_lengthProvider->set("length", _items.size());
                _itemRemoved.execute(*this, provider);
            }
        };
    }
}
