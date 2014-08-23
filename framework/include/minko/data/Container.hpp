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
		class Container
		{

		public:
			typedef Signal<Container&, const std::string&, const std::string&>	PropertyChangedSignal;

		private:
            typedef Signal<Provider::Ptr, const std::string&>	        ProviderChangedSignal;
            typedef std::list<ProviderChangedSignal::Slot>              ProviderChangedSignalSlotList;

			typedef std::shared_ptr<Provider>							ProviderPtr;
            typedef std::shared_ptr<Collection>							CollectionPtr;
			typedef std::shared_ptr<data::AbstractFilter>				AbsFilterPtr;
			typedef Signal<ProviderPtr, const std::string&>				ProviderPropertyChangedSignal;
			typedef ProviderPropertyChangedSignal::Slot					ProviderPropertyChangedSlot;
            typedef Signal<Collection&, ProviderPtr>::Slot              CollectionChangedSignalSlot;

        private:
			std::list<ProviderPtr>									_providers;
            std::list<CollectionPtr>                                _collections;
            Provider::Ptr                                           _lengthProvider;

			PropertyChangedSignal	    							_propertyAdded;
			PropertyChangedSignal     								_propertyRemoved;
            PropertyChangedSignal                                   _propertyChanged;
            std::map<std::string, PropertyChangedSignal>            _propertyNameToChangedSignal;

            std::map<ProviderPtr, ProviderChangedSignalSlotList>	_propertySlots;
            std::map<CollectionPtr, CollectionChangedSignalSlot>    _collectionItemAddedSlots;
            std::map<CollectionPtr, CollectionChangedSignalSlot>    _collectionItemRemovedSlots;

		public:
            Container();

            ~Container()
            {

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
            T*
            getUnsafePointer(const std::string& propertyName) const
            {
                auto providerAndToken = getProviderByPropertyName(propertyName);
                auto provider = std::get<0>(providerAndToken);

                assert(provider != nullptr);

                return provider->getUnsafePointer<T>(std::get<1>(providerAndToken));
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
			PropertyChangedSignal&
			propertyAdded()
			{
				return _propertyAdded;
			}

			inline
			PropertyChangedSignal&
			propertyRemoved()
			{
				return _propertyRemoved;
			}

            inline
            PropertyChangedSignal&
            propertyChanged()
            {
                return _propertyChanged;
            }

            inline
            PropertyChangedSignal&
            propertyChanged(const std::string& propertyName)
            {
                return _propertyNameToChangedSignal[propertyName];
            }

            inline
			const std::list<ProviderPtr>&
			providers() const
			{
				return _providers;
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

            inline
			bool
			hasProperty(const std::string& propertyName) const
            {
                return std::get<0>(getProviderByPropertyName(propertyName)) != nullptr;
            }

            inline
            bool
            hasPropertyChangedSignal(const std::string& propertyName) const
            {
                return _propertyNameToChangedSignal.count(propertyName) != 0;
            }

            static
            const std::string
            getActualPropertyName(const std::unordered_map<std::string, std::string>&   variables,
                                  const std::string&                                    propertyName);

		private:
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

            void
            updateCollectionLength(CollectionPtr collection);
		};
	}
}
