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
#include "minko/Any.hpp"
#include "minko/Signal.hpp"
#include "minko/data/Value.hpp"

namespace minko
{
	namespace data
	{			
		class Provider :
			public std::enable_shared_from_this<Provider>
		{
		public:
			typedef std::shared_ptr<Provider> Ptr;

		private:
			template <typename P>
			class ValueWrapper;

			typedef Signal<std::shared_ptr<Value>>::Slot ChangedSignalSlot;

		private:

			std::vector<std::string>								_names;
			std::unordered_map<std::string, std::shared_ptr<Value>>	_values;
			std::unordered_map<std::string, ChangedSignalSlot>		_changedSignalSlots;

			std::shared_ptr<Signal<Ptr, const std::string&>>		_propertyChanged;
			std::shared_ptr<Signal<Ptr, const std::string&>>		_propertyAdded;
			std::shared_ptr<Signal<Ptr, const std::string&>>		_propertyRemoved;

		public:
			static
			Ptr
			create()
			{
				return std::shared_ptr<Provider>(new Provider());
			}

			inline
			const std::vector<std::string>&
			propertyNames()
			{
				return _names;
			}

			inline
			const std::unordered_map<std::string, std::shared_ptr<Value>>&
			values()
			{
				return _values;
			}

			inline
			const std::string&
			propertyName(const unsigned int propertyIndex)
			{
				return _names[propertyIndex];
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyChanged()
			{
				return _propertyChanged;
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyAdded()
			{
				return _propertyAdded;
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyRemoved()
			{
				return _propertyRemoved;
			}

			template <typename T>
			typename std::enable_if<std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type
			get(const std::string& propertyName)
			{
				return std::dynamic_pointer_cast<typename T::element_type>(_values[propertyName]);
			}

			template <typename T>
			typename std::enable_if<!std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type
			get(const std::string& propertyName)
			{
				return std::dynamic_pointer_cast<ValueWrapper<T>>(_values[propertyName])->value();
			}

			template <typename T>
			Ptr
			set(const std::string& propertyName, T value)
			{
				registerProperty(propertyName, wrapProperty<T>(value));

				return shared_from_this();
			}

			void
			unset(const std::string& propertyName);

		private:
			Provider() :
				enable_shared_from_this(),
				_propertyChanged(Signal<Ptr, const std::string&>::create()),
				_propertyAdded(Signal<Ptr, const std::string&>::create()),
				_propertyRemoved(Signal<Ptr, const std::string&>::create())
			{
			}

			void
			propertyWrapperInitHandler(const std::string& propertyName);

			template <typename T>
			inline
			std::shared_ptr<Value>
			wrapProperty(typename std::enable_if<std::is_convertible<T, std::shared_ptr<Value>>::value, std::shared_ptr<Value>>::type	value)
			{
				return value;
			}

			template <typename T>
			inline
			std::shared_ptr<Value>
			wrapProperty(typename std::enable_if<!std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type	value)
			{
				return ValueWrapper<T>::create(value);
			}

			void
			registerProperty(const std::string& propertyName, std::shared_ptr<Value> value);

			template <typename P>
			class ValueWrapper :
				public Value,
				public std::enable_shared_from_this<Value>
			{
			public:
				typedef std::shared_ptr<ValueWrapper> Ptr;

			private:
				P _value;

			public:
				inline static
				Ptr
				create(P value)
				{
					return std::shared_ptr<ValueWrapper>(new ValueWrapper(value));
				}

				inline
				P
				value()
				{
					return _value;
				}

			private:
				ValueWrapper(P value) :
					_value(value)
				{
				}
			};
		};
	}
}
