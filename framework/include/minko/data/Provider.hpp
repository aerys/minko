/*
Copyright (c) 2014 Aerys

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
#include "minko/Flyweight.hpp"
#include "minko/Uuid.hpp"

#include "sparsehash/forward.h"

namespace minko
{
	namespace data
	{
		class Provider :
			public std::enable_shared_from_this<Provider>,
            public Uuid::enable_uuid
		{
        private:
#ifdef MINKO_USE_SPARSE_HASH_MAP
			template <typename... H>
			using map = google::sparse_hash_map<H...>;
#else
			template <class K, typename... V>
			using map = std::unordered_map<K, V...>;
#endif

            template <typename T>
            struct is_shared_ptr : std::false_type {};
            template <typename T>
            struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

            template <typename T>
            struct is_weak_ptr : std::false_type {};
            template <typename T>
            struct is_weak_ptr<std::weak_ptr<T>> : std::true_type{};

            template <typename T>
            struct is_valid {
                static const bool value = !std::is_pointer<T>::value && !std::is_reference<T>::value
                    && !is_shared_ptr<T>::value &&!is_weak_ptr<T>::value;
            };

		public:
			typedef std::shared_ptr<Provider>	            Ptr;
			typedef Flyweight<std::string>		            PropertyName;
			typedef std::pair<PropertyName, Any>            ValueType;
			typedef map<PropertyName, Any*> 		        ValueMap;
			typedef std::unordered_map<PropertyName, Any*> 	DefaultValueMap;

		private:
            ValueMap*							            _values;

			Signal<Ptr, const PropertyName&>                _propertyAdded;
            Signal<Ptr, const PropertyName&>	            _propertyChanged;
			Signal<Ptr, const PropertyName&>	            _propertyRemoved;

		public:
			static
			Ptr
			create()
			{
				Ptr provider = std::make_shared<Provider>();

				return provider;
			}

			static
			Ptr
			create(const std::string& uuid)
			{
				Ptr provider = std::make_shared<Provider>(uuid);

				return provider;
			}

			static
			Ptr
			create(const DefaultValueMap& values)
			{
				Ptr provider = std::make_shared<Provider>(values);

				return provider;
			}

			static
			Ptr
			create(Ptr source)
			{
				return create()->copyFrom(source);
			}

			bool
            hasProperty(const PropertyName& propertyName) const;

			inline
            const ValueMap&
			values() const
			{
				return *_values;
			}

			inline
			Signal<Ptr, const PropertyName&>&
			propertyAdded()
			{
				return _propertyAdded;
			}

            inline
			Signal<Ptr, const PropertyName&>&
			propertyChanged()
			{
				return _propertyChanged;
			}

			inline
			Signal<Ptr, const PropertyName&>&
			propertyRemoved()
			{
				return _propertyRemoved;
			}

			template <typename T>
			inline
            typename std::enable_if<is_valid<T>::value, const T&>::type
            get(const PropertyName& propertyName) const
			{
                return *Any::unsafe_cast<T>(getValue(propertyName));
			}

            template <typename T>
            inline
            typename std::enable_if<is_valid<T>::value, const T*>::type
            getPointer(const PropertyName& propertyName) const
            {
                return Any::unsafe_cast<T>(getValue(propertyName));
            }

            template <typename T>
            inline
            typename std::enable_if<is_valid<T>::value, T*>::type
            getUnsafePointer(const PropertyName& propertyName)
            {
                return Any::unsafe_cast<T>(getValue(propertyName));
            }

            template <typename T>
            typename std::enable_if<is_valid<T>::value, Ptr>::type
            set(const PropertyName& propertyName, T value)
            {
                if (hasProperty(propertyName))
                {
                    T* ptr = Any::cast<T>(getValue(propertyName));

#if DEBUG
                    if (!ptr)
                        throw std::invalid_argument("Property `" + *propertyName + "` does not exist or has an incorrect type.");
#endif

                    auto changed = !(*ptr == value);

                    *ptr = value;
					// memcpy(ptr, &value, sizeof(T));
                    if (changed)
                        _propertyChanged.execute(shared_from_this(), propertyName);
                }
                else
                {
                    setValue(propertyName, new Any(value));
                    _propertyAdded.execute(shared_from_this(), propertyName);
                    _propertyChanged.execute(shared_from_this(), propertyName);
                }

                return shared_from_this();
            }

            Ptr
            set(std::initializer_list<data::Provider::ValueType> values);

            template <typename T>
			bool
            propertyHasType(const PropertyName& propertyName) const
			{
				return Any::cast<T>(getValue(propertyName)) != nullptr;
			}

			virtual
			Ptr
            unset(const std::string& propertyName);

			Ptr
			clone();

			virtual
			Ptr
			copyFrom(Ptr source);

            void
            merge(Ptr source);

            ~Provider();

			Provider();
            
            explicit
            Provider(const std::string& uuid);

			Provider(const DefaultValueMap& values);

        private:
            Any*
            getValue(const PropertyName& propertyName) const;

            void
            setValue(const PropertyName& propertyName, Any* value);

		};
	}
}
