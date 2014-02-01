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
			std::unordered_map<std::string, Any>					_values;
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
			const std::unordered_map<std::string, Any>&
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
		    T
			get(const std::string& propertyName, bool skipPropertyNameFormatting)
			{
				const std::string&	formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				auto				foundIt			= values().find(formattedName);

				if (foundIt == values().end())
					throw std::invalid_argument("propertyName");
				
				return Any::cast<T>(foundIt->second);
			}

			template <typename T>
			inline
			T
			get(const std::string& propertyName)
			{
				return get<T>(propertyName, false);
			}

			template <typename T>
			bool
			propertyHasType(const std::string& propertyName, bool skipPropertyNameFormatting = false) const
			{
				const std::string&	formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				const auto			foundIt			= _values.find(formattedName);

				if (foundIt == _values.end())
					throw std::invalid_argument("propertyName");

				try
				{
					Any::cast<T>(foundIt->second);
				}
				catch (...)
				{
					return false;
				}
				
				return true;
			}

			template <typename T>
			typename std::enable_if<!std::is_convertible<T, Value::Ptr>::value, Provider::Ptr>::type
			set(const std::string& propertyName, T value, bool skipPropertyNameFormatting)
			{
				auto		formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				
				const auto	foundValueIt	= _values.find(formattedName);
				const bool	isNewValue		= foundValueIt == _values.end();
				//const bool	changed			= !isNewValue;// || !((*value) == (*foundValueIt->second));
	
				_values[formattedName] = value;
		
				if (isNewValue)
				{
					_names.push_back(formattedName);

					_propertyAdded->execute(shared_from_this(), formattedName);
				}

				//if (changed)
				{
					_propReferenceChanged->execute(shared_from_this(), formattedName);
					_propValueChanged->execute(shared_from_this(), formattedName);
				}

				return shared_from_this();
			}

			template <typename T>
			typename std::enable_if<std::is_convertible<T, Value::Ptr>::value, Provider::Ptr>::type
			set(const std::string& propertyName, T value, bool skipPropertyNameFormatting)
			{
				auto		formattedName	= skipPropertyNameFormatting ? propertyName : formatPropertyName(propertyName);
				
				const auto	foundValueIt	= _values.find(formattedName);
				const bool	isNewValue		= (foundValueIt == _values.end());
				//const bool	changed			= !isNewValue;// || !((*value) == (*foundValueIt->second));
	
				_values[formattedName] = value;
				
				if (isNewValue)
				{
#if defined(EMSCRIPTEN)
					auto that = shared_from_this();
					_valueChangedSlots[formattedName] = value->changed()->connect([&, that, formattedName, this](Value::Ptr)
					{
						_propValueChanged->execute(that, formattedName);
					});
#else
					_valueChangedSlots[formattedName] = value->changed()->connect(std::bind(
					     &Signal<Provider::Ptr, const std::string&>::execute,
						 _propValueChanged,
						 shared_from_this(),
						 formattedName
					));
#endif
					
					_names.push_back(formattedName);
					
					_propertyAdded->execute(shared_from_this(), formattedName);
				}

				//if (changed)
				{
					_propReferenceChanged->execute(shared_from_this(), formattedName);
					_propValueChanged->execute(shared_from_this(), formattedName);
				}
				
				return shared_from_this();				
			}

			template <typename T>
			inline
			Ptr
			set(const std::string& propertyName, T value)
			{
				return set(propertyName, value, false);
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
			
		};
	}
}
