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
			std::unordered_map<std::string, ChangedSignalSlot>		_valueChangedSlots;
			std::unordered_map<std::string, ChangedSignalSlot>		_referenceChangedSlots;

			std::shared_ptr<Signal<Ptr, const std::string&>>		_propertyAdded;
			std::shared_ptr<Signal<Ptr, const std::string&>>		_propValueChanged;
			std::shared_ptr<Signal<Ptr, const std::string&>>		_propReferenceChanged;
			std::shared_ptr<Signal<Ptr, const std::string&>>		_propertyRemoved;

		public:
			static const std::string NO_STRUCT_SEP;

		public:
			static
			Ptr
			create()
			{
				return std::shared_ptr<Provider>(new Provider());
			}

			static
			Ptr
			create(Ptr source)
			{
				return create()->copyFrom(source);
			}

			inline
			const std::vector<std::string>&
			propertyNames() const
			{
				return _names;
			}

			virtual
			bool 
			hasProperty(const std::string&, bool skipPropertyNameFormatting = false) const;

			inline
			const std::unordered_map<std::string, std::shared_ptr<Value>>&
			values() const
			{
				return _values;
			}

			inline
			const std::string&
			propertyName(const unsigned int propertyIndex) const
			{
				return _names[propertyIndex];
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyValueChanged() const
			{
				return _propValueChanged;
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyReferenceChanged() const
			{
				return _propReferenceChanged;
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyAdded() const
			{
				return _propertyAdded;
			}

			inline
			std::shared_ptr<Signal<Ptr, const std::string&>>
			propertyRemoved() const
			{
				return _propertyRemoved;
			}

			template <typename T>
			typename std::enable_if<std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type
			get(const std::string& propertyName, bool skipPropertyNameFormatting) const
			{
				const std::string&	formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				auto				foundIt			= values().find(formattedName);

				return foundIt != values().end()
					? std::dynamic_pointer_cast<typename T::element_type>(foundIt->second)
					: nullptr;

				// return std::dynamic_pointer_cast<typename T::element_type>(_values[formattedName]);
			}

			/*
			template <typename T>
			typename std::enable_if<std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type
			get(const std::string& propertyName)
			{
				return get<T>(propertyName, false);
			}
			*/

			template <typename T>
			typename std::enable_if<!std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type
			get(const std::string& propertyName, bool skipPropertyNameFormatting) const
			{
				const std::string&	formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				auto				foundIt			= values().find(formattedName);

				return foundIt != values().end()
					? std::dynamic_pointer_cast<ValueWrapper<T>>(foundIt->second)->value()
					: T();

				// return std::dynamic_pointer_cast<ValueWrapper<T>>(_values[formattedName])->value();
			}

			template <typename T>
			inline
			T
			get(const std::string& propertyName) const
			{
				return get<T>(propertyName, false);
			}

			template <typename T>
			typename std::enable_if<std::is_convertible<T, std::shared_ptr<Value>>::value, bool>::type
			propertyHasType(const std::string& propertyName, bool skipPropertyNameFormatting = false) const
			{
				const std::string&	formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				const auto			foundPropertyIt	= _values.find(formattedName);

				return foundPropertyIt != _values.end() 
					? std::dynamic_pointer_cast<typename T::element_type>(foundPropertyIt->second) != nullptr
					: false;
			}

			template <typename T>
			typename std::enable_if<!std::is_convertible<T, std::shared_ptr<Value>>::value, bool>::type
			propertyHasType(const std::string& propertyName, bool skypPropertyNameFormatting = false) const
			{
				const std::string&	formattedName	= skypPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				const auto			foundPropertyIt	= _values.find(formattedName);

				return foundPropertyIt != _values.end() 
					? std::dynamic_pointer_cast<ValueWrapper<T>>(foundPropertyIt->second) != nullptr
					: false;
			}

			template <typename T>
			Ptr
			set(const std::string& propertyName, T value, bool skipPropertyNameFormatting)
			{
				registerProperty(
					skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName),
					wrapProperty<T>(value)
				);

				return shared_from_this();
			}

			template <typename T>
			inline
			Ptr
			set(const std::string& propertyName, T value)
			{
				return set<T>(propertyName, value, false);
			}

			virtual
			void
			unset(const std::string& propertyName);

			void
			swap(const std::string& propertyName1, const std::string& propertyName2, bool skipPropertyNameFormatting = false);

			Ptr
			copyFrom(Ptr source);

		protected:
			Provider();

			virtual
			void
			registerProperty(const std::string& propertyName, std::shared_ptr<Value> value);

			virtual
			std::string
			formatPropertyName(const std::string& propertyName) const
			{
				return propertyName;
			}

			virtual
			std::string
			unformatPropertyName(const std::string& propertyName) const
			{
				return propertyName;
			}

		private:

			template <typename T>
			inline
			std::shared_ptr<Value>
			wrapProperty(typename std::enable_if<std::is_convertible<T, std::shared_ptr<Value>>::value, std::shared_ptr<Value>>::type	value) const
			{
				return value;
			}

			template <typename T>
			inline
			std::shared_ptr<Value>
			wrapProperty(typename std::enable_if<!std::is_convertible<T, std::shared_ptr<Value>>::value, T>::type	value) const
			{
				return ValueWrapper<T>::create(value);
			}

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
				value() const
				{
					return _value;
				}

				bool
				operator==(const Value& value) const
				{
					const ValueWrapper<P>* x = dynamic_cast<const ValueWrapper<P>*>(&value);

					return x ? _value == x->value() : false;
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
