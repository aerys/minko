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
#include "minko/Flyweight.hpp"
#include "minko/data/Provider.hpp"

#include "sparsehash/forward.h"

namespace minko
{
	namespace data
	{
		class Store
		{
		public:
			typedef Flyweight<std::string>								PropertyName;
            typedef Signal<Provider::Ptr, const PropertyName&>	        ProviderChangedSignal;

		private:
            typedef std::list<ProviderChangedSignal::Slot>              ProviderChangedSignalSlotList;

			typedef std::shared_ptr<Provider>						    ProviderPtr;
            typedef std::shared_ptr<Collection>						    CollectionPtr;
			typedef std::shared_ptr<data::AbstractFilter>			    AbsFilterPtr;
			typedef Signal<ProviderPtr, const PropertyName&>			ProviderPropertyChangedSignal;
			typedef ProviderPropertyChangedSignal::Slot				    ProviderPropertyChangedSlot;
            typedef Signal<Collection&, ProviderPtr>::Slot              CollectionChangedSignalSlot;
            typedef Flyweight<std::string>                              FString;
			typedef std::list<std::pair<FString, FString>>				FStringList;

#ifdef MINKO_USE_SPARSE_HASH_MAP
            template <typename... H>
            using map = google::sparse_hash_map<H...>;
#else
            template <class K, typename... V>
            using map = std::unordered_map<K, V...>;
#endif

        public:
            typedef Signal<Store&, ProviderPtr, const PropertyName&>	PropertyChangedSignal;

        private:
            typedef map<PropertyName, PropertyChangedSignal*> 			ChangedSignalMap;
            typedef map<ProviderPtr, ProviderChangedSignalSlotList> 	ProviderToChangedSlotListMap;
            typedef map<CollectionPtr, CollectionChangedSignalSlot> 	CollectionToChangedSlotMap;

        private:
			std::list<ProviderPtr>			_providers;
            std::list<CollectionPtr>        _collections;
            Provider::Ptr                   _lengthProvider;

			PropertyChangedSignal	    	_propertyAdded;
			PropertyChangedSignal     		_propertyRemoved;
            PropertyChangedSignal           _propertyChanged;
            ChangedSignalMap*            	_propertyNameToChangedSignal;
            ChangedSignalMap*            	_propertyNameToAddedSignal;
            ChangedSignalMap*            	_propertyNameToRemovedSignal;

            ProviderToChangedSlotListMap*	_propertySlots;
            CollectionToChangedSlotMap*     _collectionItemAddedSlots;
            CollectionToChangedSlotMap*     _collectionItemRemovedSlots;

		public:
            Store();

            Store(Store&& store);

            Store(const Store& store);

			Store(const Store& store, bool deepCopy);

            Store& operator=(Store&&);

            ~Store();

            template <typename T>
			bool
			propertyHasType(const PropertyName& propertyName) const
			{
                auto providerAndToken = getProviderByPropertyName(*propertyName);
                auto provider = std::get<0>(providerAndToken);

                if (provider == nullptr)
                    throw;

				return provider->propertyHasType<T>(std::get<1>(providerAndToken));
			}

			template <typename T>
			const T&
			get(const PropertyName& propertyName) const
			{
                auto providerAndToken = getProviderByPropertyName(*propertyName);
                auto provider = std::get<0>(providerAndToken);

                if (provider == nullptr)
                    throw;

                return provider->get<T>(std::get<1>(providerAndToken));
			}

            template <typename T>
            const T*
            getPointer(const PropertyName& propertyName) const
            {
                auto providerAndToken = getProviderByPropertyName(*propertyName);
                auto provider = std::get<0>(providerAndToken);

                if (provider == nullptr)
                    throw;

                return provider->getPointer<T>(std::get<1>(providerAndToken));
            }

            template <typename T>
            T*
            getUnsafePointer(const PropertyName& propertyName) const
            {
                auto providerAndToken = getProviderByPropertyName(*propertyName);
                auto provider = std::get<0>(providerAndToken);

                if (provider == nullptr)
                {
                    return nullptr;
                    //throw;
                }

                return provider->getUnsafePointer<T>(std::get<1>(providerAndToken));
            }

			template <typename T>
			void
			set(const PropertyName& propertyName, T value)
			{
                auto providerAndToken = getProviderByPropertyName(*propertyName);
                auto provider = std::get<0>(providerAndToken);

                if (provider == nullptr)
                    throw;

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
            propertyAdded(const PropertyName& propertyName)
            {
                return getOrInsertSignal(_propertyNameToAddedSignal, propertyName);
            }

            inline
            PropertyChangedSignal&
            propertyRemoved(const PropertyName& propertyName)
            {
                return getOrInsertSignal(_propertyNameToRemovedSignal, propertyName);
            }

            inline
            PropertyChangedSignal&
            propertyChanged(const PropertyName& propertyName)
            {
                return getOrInsertSignal(_propertyNameToChangedSignal, propertyName);
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
            hasProperty(const PropertyName& propertyName) const
            {
                return std::get<0>(getProviderByPropertyName(*propertyName)) != nullptr;
            }

            bool
            hasPropertyAddedSignal(const PropertyName& propertyName) const;

            bool
            hasPropertyRemovedSignal(const PropertyName& propertyName) const;

            bool
            hasPropertyChangedSignal(const PropertyName& propertyName) const;

            static
            const std::string
            getActualPropertyName(const FStringList& variables, const FString& propertyName);

		private:
			std::pair<ProviderPtr, std::string>
            getProviderByPropertyName(const std::string& propertyName) const;

			void
			providerPropertyAddedHandler(ProviderPtr        provider,
                                         CollectionPtr      collection,
                                         const FString& 	propertyName);

			void
			providerPropertyRemovedHandler(ProviderPtr          provider,
                                           CollectionPtr        collection,
                                           const FString&   	propertyName);

            void
            doAddProvider(ProviderPtr provider, CollectionPtr collection = nullptr);

            void
            doRemoveProvider(ProviderPtr provider, CollectionPtr collection = nullptr);

            PropertyName
            formatPropertyName(CollectionPtr        collection,
                               ProviderPtr          provider,
                               const std::string&   propertyName,
                               bool                 useUuid = false);

            PropertyName
            formatPropertyName(CollectionPtr collection, const std::string& index, const std::string& propertyName);

            void
            addProviderToCollection(std::shared_ptr<data::Provider> provider,
                                    const std::string&              collectionName);

            void
            removeProviderFromCollection(std::shared_ptr<data::Provider> provider,
                                         const std::string&              collectionName);

            void
            updateCollectionLength(CollectionPtr collection);

            void
            executePropertySignal(ProviderPtr                   provider,
                                  CollectionPtr                 collection,
                                  const PropertyName&           propertyName,
								  const PropertyChangedSignal&  anyChangedSignal,
                                  const ChangedSignalMap& 		propertyNameToSignal);

            void
            copyFrom(const Store& store, bool deepCopy = false);

            PropertyChangedSignal&
            getOrInsertSignal(ChangedSignalMap* signals, const PropertyName& propertyName);

            void
            initialize();
		};
	}
}
