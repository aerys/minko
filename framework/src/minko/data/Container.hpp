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
			typedef std::shared_ptr<Container>	Ptr;
			typedef Signal<Ptr, const std::string&>	PropertyChangedSignal;

		private:
			typedef std::shared_ptr<Provider>									ProviderPtr;
			typedef std::shared_ptr<Signal<Ptr, const std::string&>>				PropertyChangedSignalPtr;
			typedef Signal<std::shared_ptr<Provider>, const std::string&>::Slot	ProviderPropertyChangedSlot;

			std::list<ProviderPtr>											_providers;
			std::unordered_map<std::string, ProviderPtr>					_propertyNameToProvider;

			std::unordered_map<std::string, PropertyChangedSignalPtr>			_propertyChanged;

			std::unordered_map<ProviderPtr, std::list<Any>>					_propertyAddedOrRemovedSlots;
			std::unordered_map<ProviderPtr, ProviderPropertyChangedSlot>	_ProviderPropertyChangedSlot;

		public:
			static
			Ptr
			create()
			{
				return std::shared_ptr<Container>(new Container());
			}

			void
			addProvider(std::shared_ptr<Provider> provider);

			void
			removeProvider(std::shared_ptr<Provider> provider);

			bool
			hasProvider(std::shared_ptr<Provider> provider);

			bool
			hasProperty(const std::string& propertyName);

			template <typename T>
			T
			get(const std::string& propertyName)
			{
				assertPropertyExists(propertyName);

				return _propertyNameToProvider[propertyName]->get<T>(propertyName);
			}

			template <typename T>
			void
			set(const std::string& propertyName, T value)
			{
				assertPropertyExists(propertyName);

				_propertyNameToProvider[propertyName]->set<T>(propertyName, value);
			}

			PropertyChangedSignalPtr
			propertyChanged(const std::string& propertyName);

			inline
			const std::list<ProviderPtr>&
			providers()
			{
				return _providers;
			}

		private:
			Container();

			void
			assertPropertyExists(const std::string& propertyName);

			void 
			ProviderPropertyChangedHandler(std::shared_ptr<Provider> 	provider,
												const std::string& 				propertyName);

			void
			ProviderPropertyAddedHandler(std::shared_ptr<Provider> 	provider,
												const std::string& 			propertyName);

			void
			ProviderPropertyRemovedHandler(std::shared_ptr<Provider> 	provider,
												const std::string& 				propertyName);
		};
	}
}
