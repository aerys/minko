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
			typedef Signal<Ptr, const std::string&>						PropertyChangedSignal;

		private:
			typedef std::shared_ptr<PropertyChangedSignal>				PropertyChangedSignalPtr;

			typedef std::shared_ptr<Provider>							ProviderPtr;
			typedef std::shared_ptr<data::AbstractFilter>				AbsFilterPtr;
			typedef Signal<ProviderPtr, const std::string&>				ProviderPropertyChangedSignal;
			typedef ProviderPropertyChangedSignal::Slot					ProviderPropertyChangedSlot;

        private:
            static uint CONTAINER_ID;

			std::list<ProviderPtr>										_providers;
            std::unordered_map<ProviderPtr, uint>                       _numUses;
			std::shared_ptr<Provider>									_arrayLengths;
            std::unordered_map<std::string, ProviderPtr>                _propertyNameToProvider;

			PropertyChangedSignalPtr									_propertyAdded;
			PropertyChangedSignalPtr									_propertyRemoved;
            std::unordered_map<std::string, PropertyChangedSignalPtr>   _propertyChanged;
            Signal<Ptr, ProviderPtr>::Ptr								_providerAdded;
            Signal<Ptr, ProviderPtr>::Ptr								_providerRemoved;

            std::unordered_map<ProviderPtr, std::list<Any>>				_propertySlots;
            std::unordered_map<std::string, PropertyChangedSignalPtr>	_propertyChangedSlots;

		public:
			uint _containerId;
			static
			Ptr
			create()
			{
				auto container = std::shared_ptr<Container>(new Container());

				container->initialize();

				return container;
			}

			void
			initialize();

			void
			addProvider(std::shared_ptr<Provider> provider);

			void
			removeProvider(std::shared_ptr<Provider> provider);

			bool
			hasProvider(std::shared_ptr<Provider> provider) const;

			bool
			hasProperty(const std::string&) const;
			
            template <typename T>
			bool
			propertyHasType(const std::string& propertyName, bool skipPropertyNameFormatting = false) const
			{
				assertPropertyExists(propertyName);

				const auto& provider = _propertyNameToProvider.find(propertyName)->second;

				return provider->propertyHasType<T>(propertyName, skipPropertyNameFormatting);
			}

			template <typename T>
			const T&
			get(const std::string& propertyName) const
			{
				assertPropertyExists(propertyName);

				const auto& provider = _propertyNameToProvider.find(propertyName)->second;

				return provider->get<T>(propertyName, true);
			}

            template <typename T>
            const T*
            getPointer(const std::string& propertyName) const
            {
                assertPropertyExists(propertyName);

                const auto& provider = _propertyNameToProvider.find(propertyName)->second;

                return provider->getPointer<T>(propertyName, true);
            }

			template <typename T>
			void
			set(const std::string& propertyName, T value)
			{
				assertPropertyExists(propertyName);

				auto provider = _propertyNameToProvider[propertyName];

				provider->set<T>(propertyName, value);
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
                    _propertyChanged[propertyName] = Signal<Container::Ptr, const std::string&>::create();

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

			Ptr
			filter(const std::set<AbsFilterPtr>&, Ptr = nullptr) const;

		private:
			Container();

			void
			assertPropertyExists(const std::string& propertyName) const;

			void
			providerPropertyAddedHandler(ProviderPtr, const std::string& propertyName);

			void
			providerPropertyRemovedHandler(ProviderPtr, const std::string& propertyName);

            void
            providerPropertyChangedHandler(ProviderPtr, const std::string& propertyName);

			inline
			void
			assertProviderDoesNotExist(std::shared_ptr<Provider> provider) const
			{
#ifdef DEBUG
				if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
					throw std::invalid_argument("provider");
#endif // DEBUG
			}

			inline
			void
			assertProviderExists(std::shared_ptr<Provider> provider) const
			{
#ifdef DEBUG
				if (std::find(_providers.begin(), _providers.end(), provider) == _providers.end())
					throw std::invalid_argument("provider");
#endif // DEBUG
			}
		};
	}
}
