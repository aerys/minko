#pragma once

#include "minko/Common.hpp"
#include "minko/Any.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/data/Value.hpp"

namespace minko
{
	namespace scene
	{
		namespace data
		{			
			class DataProvider :
				public std::enable_shared_from_this<DataProvider>
			{
			public:
				typedef std::shared_ptr<DataProvider> ptr;

			private:
				template <typename P>
				class ValueWrapper;

				typedef Signal<std::shared_ptr<Value>>::Slot ChangedSignalSlot;

			private:

				std::vector<std::string>								_names;
				std::unordered_map<std::string, std::shared_ptr<Value>>	_values;
				std::unordered_map<std::string, ChangedSignalSlot>		_changedSignalSlots;

				std::shared_ptr<Signal<ptr, const std::string&>>		_propertyChanged;
				std::shared_ptr<Signal<ptr, const std::string&>>		_propertyAdded;
				std::shared_ptr<Signal<ptr, const std::string&>>		_propertyRemoved;

			public:
				static
				ptr
				create()
				{
					return std::shared_ptr<DataProvider>(new DataProvider());
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
				std::shared_ptr<Signal<ptr, const std::string&>>
				propertyChanged()
				{
					return _propertyChanged;
				}

				inline
				std::shared_ptr<Signal<ptr, const std::string&>>
				propertyAdded()
				{
					return _propertyAdded;
				}

				inline
				std::shared_ptr<Signal<ptr, const std::string&>>
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
				ptr
				set(const std::string& propertyName, T value)
				{
					registerProperty(propertyName, wrapProperty<T>(value));

					return shared_from_this();
				}

				void
				unset(const std::string& propertyName);

			private:
				DataProvider() :
					enable_shared_from_this(),
					_propertyChanged(Signal<ptr, const std::string&>::create()),
					_propertyAdded(Signal<ptr, const std::string&>::create()),
					_propertyRemoved(Signal<ptr, const std::string&>::create())
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
					typedef std::shared_ptr<ValueWrapper> ptr;

				private:
					P _value;

				public:
					inline static
					ptr
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
}
