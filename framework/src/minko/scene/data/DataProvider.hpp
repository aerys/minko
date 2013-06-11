#pragma once

#include "minko/Common.hpp"
#include "minko/Any.hpp"
#include "minko/Signal.hpp"

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
				class DataProviderPropertyWrapper;

				typedef std::shared_ptr<DataProviderPropertyWrapper>	WrapperPtr;

				std::vector<std::string>							_names;
				std::map<std::string, WrapperPtr>					_values;

				std::shared_ptr<Signal<ptr, const std::string&>>	_propertyChanged;
				std::shared_ptr<Signal<ptr, const std::string&>>	_propertyAdded;
				std::shared_ptr<Signal<ptr, const std::string&>>	_propertyRemoved;

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
				const std::map<std::string, WrapperPtr>&
				values()
				{
					return _values;
				}

				inline
				const unsigned int
				numProperties()
				{
					return _names.size();
				}

				inline
				const std::string&
				getPropertyName(const unsigned int propertyIndex)
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

				inline
				DataProviderPropertyWrapper&
				operator[](const std::string& propertyName)
				{
					if (_values.count(propertyName) == 0)
						_values[propertyName] = DataProviderPropertyWrapper::create(
							std::bind(&DataProvider::propertyWrapperInitHandler, shared_from_this(), propertyName)
						);

					return *_values[propertyName];
				}

				template <typename T>
				T
				getProperty(const std::string& propertyName)
				{
					return Any::cast<T>(*_values[propertyName]);
				}

				template <typename T>
				ptr
				setProperty(const std::string& propertyName, T value)
				{
					bool isNewValue = _values.count(propertyName) == 0;

					_values[propertyName] = DataProviderPropertyWrapper::create(
						value,
						std::bind(
							&Signal<ptr, const std::string&>::execute,
							_propertyChanged,
							shared_from_this(),
							propertyName
						)
					);

					if (isNewValue)
					{
						_names.push_back(propertyName);
						_propertyAdded->execute(shared_from_this(), propertyName);
					}
					else
						_propertyChanged->execute(shared_from_this(), propertyName);

					return shared_from_this();
				}

				void
				unsetProperty(const std::string& propertyName);

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
	
				class DataProviderPropertyWrapper :
					public Any,
					public std::enable_shared_from_this<DataProviderPropertyWrapper>
				{
				public:
					typedef std::shared_ptr<DataProviderPropertyWrapper>	ptr;

				private:
					std::function<void(void)> _changedCallack;

				public:
					inline static
					ptr
					create(const Any& value, std::function<void(void)> changedCallback)
					{
						return std::shared_ptr<DataProviderPropertyWrapper>(
							new DataProviderPropertyWrapper(value, changedCallback)
						);
					}

					inline static
					ptr
					create(std::function<void(void)> changedCallback)
					{
						return std::shared_ptr<DataProviderPropertyWrapper>(
							new DataProviderPropertyWrapper(changedCallback)
						);
					}	

				  	template <typename ValueType>
					Any&
					operator=(const ValueType& rhs)
					{
					    Any(rhs).swap(*this);

						_changedCallack();

					    return *this;
					}

				private:
					DataProviderPropertyWrapper(std::function<void(void)> changedCallback) :
						Any(),
						enable_shared_from_this(),
						_changedCallack(changedCallback)
					{
					}

					DataProviderPropertyWrapper(const Any& value, std::function<void(void)> changedCallback) :
						Any(value),
						enable_shared_from_this(),
						_changedCallack(changedCallback)
					{
					}
				};
			};
		}
	}
}
