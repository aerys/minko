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

#pragma once

#include "minko/Common.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
	namespace data
	{
		class Container :
			public std::enable_shared_from_this<Container>
		{

		public:
			typedef std::shared_ptr<Container>							Ptr;
			typedef Signal<Ptr, const std::string&, const std::string&>	PropertyChangedSignal;

		private:
			typedef std::shared_ptr<PropertyChangedSignal>				PropertyChangedSignalPtr;

			typedef std::shared_ptr<Provider>							ProviderPtr;
            typedef std::shared_ptr<Collection>							CollectionPtr;
			typedef std::shared_ptr<data::AbstractFilter>				AbsFilterPtr;
			typedef Signal<ProviderPtr, const std::string&>				ProviderPropertyChangedSignal;
			typedef ProviderPropertyChangedSignal::Slot					ProviderPropertyChangedSlot;
            typedef Signal<Collection&, ProviderPtr>::Slot              CollectionChangedSignalSlot;

        private:
			std::list<ProviderPtr>										    _providers;
            std::list<CollectionPtr>                                        _collections;

			PropertyChangedSignalPtr									    _propertyAdded;
			PropertyChangedSignalPtr									    _propertyRemoved;
            std::unordered_map<std::string, PropertyChangedSignalPtr>       _propertyChanged;
            Signal<Ptr, ProviderPtr>::Ptr								    _providerAdded;
            Signal<Ptr, ProviderPtr>::Ptr								    _providerRemoved;
            Signal<Ptr, CollectionPtr>::Ptr								    _collectionAdded;
            Signal<Ptr, CollectionPtr>::Ptr								    _collectionRemoved;

            std::unordered_map<ProviderPtr, std::list<Any>>				    _propertySlots;
            std::unordered_map<std::string, PropertyChangedSignalPtr>	    _propertyChangedSlots;
            std::unordered_map<CollectionPtr, CollectionChangedSignalSlot>  _collectionItemAddedSlots;
            std::unordered_map<CollectionPtr, CollectionChangedSignalSlot>  _collectionItemRemovedSlots;

		public:
			static
			Ptr
			create()
			{
                return std::shared_ptr<Container>(new Container());
			}

            template <typename T>
			bool
			propertyHasType(const std::string& propertyName) const
			{
                auto providerAndToken = getProviderByPropertyName(propertyName);
                auto provider = std::get<0>(providerAndToken);

                assert(provider != nullptr);

				return provider->propertyHasType<T>(std::get<1>(providerAndToken));
			}

			template <typename T>
			const T&
			get(const std::string& propertyName) const
			{
                auto providerAndToken = getProviderByPropertyName(propertyName);
                auto provider = std::get<0>(providerAndToken);

                assert(provider != nullptr);

                return provider->get<T>(std::get<1>(providerAndToken));
			}

            template <typename T>
            const T*
            getPointer(const std::string& propertyName) const
            {
                auto providerAndToken = getProviderByPropertyName(propertyName);
                auto provider = std::get<0>(providerAndToken);

                assert(provider != nullptr);

                return provider->getPointer<T>(std::get<1>(providerAndToken));
            }

			template <typename T>
			void
			set(const std::string& propertyName, T value)
			{
                auto providerAndToken = getProviderByPropertyName(propertyName);
                auto provider = std::get<0>(providerAndToken);

                assert(provider != nullptr);

                provider->set<T>(std::get<1>(providerAndToken), value);
			}

			inline
			PropertyChangedSignalPtr
			propertyAdded() const
			{
				return _propertyAdded;
			}

			inline
			PropertyChangedSignalPtr
			propertyRemoved() const
			{
				return _propertyRemoved;
			}

            inline
            PropertyChangedSignalPtr
            propertyChanged(const std::string& propertyName)
            {
                if (_propertyChanged.count(propertyName) == 0)
                    _propertyChanged[propertyName] = PropertyChangedSignal::create();

                return _propertyChanged[propertyName];
            }

			inline
			Signal<Ptr, Provider::Ptr>::Ptr
			providerAdded() const
			{
				return _providerAdded;
			}

			inline
			Signal<Ptr, Provider::Ptr>::Ptr
			providerRemoved() const
			{
				return _providerRemoved;
			}

			inline
			const std::list<ProviderPtr>&
			providers() const
			{
				return _providers;
			}

            inline
			Signal<Ptr, CollectionPtr>::Ptr
			collectionAdded() const
			{
				return _collectionAdded;
			}

			inline
			Signal<Ptr, CollectionPtr>::Ptr
			collectionRemoved() const
			{
				return _collectionRemoved;
			}

            inline
			const std::list<CollectionPtr>&
			collections() const
			{
				return _collections;
			}

            inline
            void
			addProvider(std::shared_ptr<Provider> provider)
            {
                doAddProvider(provider);
            }

            inline
            void
			addProvider(std::shared_ptr<Provider> provider, const std::string& collectionName)
            {
                addProviderToCollection(provider, collectionName);
            }

			void
			removeProvider(std::shared_ptr<Provider> provider)
            {
                doRemoveProvider(provider);
            }

            void
			removeProvider(std::shared_ptr<Provider> provider, const std::string& collectionName)
            {
                removeProviderFromCollection(provider, collectionName);
            }

            void
            addCollection(std::shared_ptr<Collection> collection);

            void
            removeCollection(std::shared_ptr<Collection> collection);


			bool
			hasProperty(const std::string& propertyName) const;

		private:
			Container();

            std::pair<ProviderPtr, std::string>
            getProviderByPropertyName(const std::string& propertyName) const;

			void
			providerPropertyAddedHandler(ProviderPtr        provider,
                                         CollectionPtr      collection,
                                         const std::string& propertyName);

			void
			providerPropertyRemovedHandler(ProviderPtr          provider,
                                           CollectionPtr        collection,
                                           const std::string&   propertyName);

            void
            providerPropertyChangedHandler(ProviderPtr          provider,
                                           CollectionPtr        collection,
                                           const std::string&   propertyName);

            void
            doAddProvider(ProviderPtr provider, CollectionPtr collection = nullptr);

            void
            doRemoveProvider(ProviderPtr provider, CollectionPtr collection = nullptr);

            std::string
            formatPropertyName(CollectionPtr collection, ProviderPtr provider, const std::string& propertyName);

            std::string
            formatPropertyName(CollectionPtr collection, uint index, const std::string& propertyName);

            void
            addProviderToCollection(std::shared_ptr<data::Provider> provider,
                                    const std::string&              collectionName);

            void
            removeProviderFromCollection(std::shared_ptr<data::Provider> provider,
                                         const std::string&              collectionName);
		};
	}
}
