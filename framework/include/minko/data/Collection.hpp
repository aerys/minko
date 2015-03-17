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
#include "minko/Flyweight.hpp"

namespace minko
{
    namespace data
    {
        class Collection
        {
            friend class Store;

        public:
            typedef std::shared_ptr<Collection>     Ptr;

        private:
            typedef std::shared_ptr<Provider>       ProviderPtr;
            typedef std::vector<ProviderPtr>        Items;

        private:
            Flyweight<std::string>              _name;
            Items                               _items;

            Signal<Collection&, ProviderPtr>    _itemAdded;
            Signal<Collection&, ProviderPtr>    _itemRemoved;

        public:
            static inline
            Ptr
            create(const Flyweight<std::string>& name)
            {
                return std::shared_ptr<Collection>(new Collection(name));
            }

            static inline
            Ptr
            create(Ptr collection, bool deepCopy = false)
            {
                auto copy = create(collection->_name);

                if (deepCopy)
                {
                    for (auto item : collection->_items)
                        copy->_items.push_back(Provider::create(item));
                }
                else
                    copy->_items = collection->_items;

                return copy;
            }

            inline
            const Flyweight<std::string>&
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
            Items::iterator
            begin()
            {
                return _items.begin();
            }

            inline
            Items::iterator
            end()
            {
                return _items.end();
            }

            inline
            Collection&
            insert(Items::iterator position, ProviderPtr provider)
            {
                _items.insert(position, provider);
                _itemAdded.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            erase(Items::iterator position)
            {
                auto provider = *position;

                _items.erase(position);
                _itemRemoved.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            remove(ProviderPtr provider)
            {
                _items.erase(std::find(_items.begin(), _items.end(), provider));
                _itemRemoved.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            pushBack(ProviderPtr provider)
            {
                _items.push_back(provider);
                _itemAdded.execute(*this, provider);

                return *this;
            }

            inline
            Collection&
            popBack()
            {
                auto provider = _items.back();

                _items.pop_back();
                _itemRemoved.execute(*this, provider);

                return *this;
            }

        private:
            Collection(const Flyweight<std::string>& name) :
                _name(name)
            {
            }
        };
    }
}
