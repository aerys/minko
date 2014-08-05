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
        private:
            typedef std::shared_ptr<Provider>                       ProviderPtr;
            typedef std::list<ProviderPtr>                          Items;
            typedef Signal<ProviderPtr, const std::string&>::Ptr    PropertySignalPtr;
            typedef Signal<ProviderPtr, const std::string&>::Slot   PropertySignalSlot;

        private:
            Items                                               _items;

            Signal<Collection&, ProviderPtr>                    _itemAdded;
            Signal<Collection&, ProviderPtr>                    _itemRemoved;

            PropertySignalPtr                                   _propertyAdded;
            PropertySignalPtr                                   _propertyChanged;
            PropertySignalPtr                                   _propertyRemoved;

            std::unordered_map<ProviderPtr, PropertySignalSlot> _propertyAddedSlot;
            std::unordered_map<ProviderPtr, PropertySignalSlot> _propertyChangedSlot;
            std::unordered_map<ProviderPtr, PropertySignalSlot> _propertyRemovedSlot;

        public:
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
            PropertySignalPtr
            propertyAdded()
            {
                return _propertyAdded;
            }

            inline
            PropertySignalPtr
            propertyChanged()
            {
                return _propertyChanged;
            }

            inline
            PropertySignalPtr
            propertyRemoved()
            {
                return _propertyRemoved;
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
            Collection&
            insert(Items::iterator position, ProviderPtr provider)
            {
                _items.insert(position, provider);
                addProvider(provider);
                _itemAdded.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            erase(Items::iterator position)
            {
                _items.erase(position);
                removeProvider(*position);
                _itemRemoved.execute(*this, *position);

                return *this;
            }

            inline
            Collection&
            push_back(ProviderPtr provider)
            {
                _items.push_back(provider);
                addProvider(provider);
                _itemAdded.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            push_front(ProviderPtr provider)
            {
                _items.push_front(provider);
                addProvider(provider);
                _itemAdded.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            pop_back()
            {
                auto provider = _items.back();

                _items.pop_back();
                removeProvider(provider);
                _itemRemoved.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            pop_front()
            {
                auto provider = _items.front();

                _items.pop_front();
                removeProvider(provider);
                _itemRemoved.execute(*this, provider);

                return *this;
            }

        private:
            void
            addProvider(ProviderPtr provider)
            {
                _propertyAddedSlot[provider] = provider->propertyAdded()->connect(_propertyAdded->execute);
                _propertyChangedSlot[provider] = provider->propertyChanged()->connect(_propertyChanged->execute);
                _propertyRemovedSlot[provider] = provider->propertyRemoved()->connect(_propertyRemoved->execute);
            }

            void
            removeProvider(ProviderPtr provider)
            {
                _propertyAddedSlot.erase(provider);
                _propertyChangedSlot.erase(provider);
                _propertyRemovedSlot.erase(provider);
            }
        };
    }
}
