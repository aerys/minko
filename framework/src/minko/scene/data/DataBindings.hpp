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
#include "minko/scene/data/DataProvider.hpp"

namespace minko
{
	namespace scene
	{
		namespace data
		{
			class DataBindings :
				public std::enable_shared_from_this<DataBindings>
			{
			public:
				typedef std::shared_ptr<DataBindings>	Ptr;
				typedef Signal<Ptr, const std::string&>	PropertyChangedSignal;

			private:
				typedef std::shared_ptr<DataProvider>									DataProviderPtr;
				typedef std::shared_ptr<Signal<Ptr, const std::string&>>				PropertyChangedSignalPtr;
				typedef Signal<std::shared_ptr<DataProvider>, const std::string&>::Slot	DataProviderPropertyChangedSlot;

				std::list<DataProviderPtr>											_providers;
				std::unordered_map<std::string, DataProviderPtr>					_propertyNameToProvider;

				std::unordered_map<std::string, PropertyChangedSignalPtr>			_propertyChanged;

				std::unordered_map<DataProviderPtr, std::list<Any>>					_propertyAddedOrRemovedSlots;
				std::unordered_map<DataProviderPtr, DataProviderPropertyChangedSlot>	_dataProviderPropertyChangedSlot;

			public:
				static
				Ptr
				create()
				{
					return std::shared_ptr<DataBindings>(new DataBindings());
				}

				void
				addProvider(std::shared_ptr<DataProvider> provider);

				void
				removeProvider(std::shared_ptr<DataProvider> provider);

				bool
				hasProvider(std::shared_ptr<DataProvider> provider);

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

				/*inline
				Any&
				operator[](const std::string& propertyName)
				{
					return get<Any&>(propertyName);
				}*/

				PropertyChangedSignalPtr
				propertyChanged(const std::string& propertyName);

				inline
				const std::list<DataProviderPtr>&
				providers()
				{
					return _providers;
				}

			private:
				DataBindings();

				void
				assertPropertyExists(const std::string& propertyName);

				void 
				dataProviderPropertyChangedHandler(std::shared_ptr<DataProvider> 	provider,
												   const std::string& 				propertyName);

				void
				dataProviderPropertyAddedHandler(std::shared_ptr<DataProvider> 	provider,
												 const std::string& 			propertyName);

				void
				dataProviderPropertyRemovedHandler(std::shared_ptr<DataProvider> 	provider,
												   const std::string& 				propertyName);
			};
		}
	}
}
