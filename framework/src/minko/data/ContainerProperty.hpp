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

namespace minko
{
	namespace data
	{
		class ContainerProperty
		{
		private:
			typedef std::shared_ptr<data::Container>	ContainerPtr;
		private:
			ContainerPtr	_container;
			std::string		_propertyName;

		public:
			inline
			ContainerProperty():
				_container(nullptr),
				_propertyName()
			{
			}

			inline
			ContainerProperty(const data::MacroBinding& binding, ContainerPtr targetData, ContainerPtr rendererData, ContainerPtr rootData):
				_container(nullptr),
				_propertyName()
			{
				initialize(binding, targetData, rendererData, rootData);
			}

			inline
			ContainerProperty(const std::string& propertyName, ContainerPtr container):
				_container(container),
				_propertyName(propertyName)
			{
				assert(container);
			}

			inline
			const std::string& 
			name() const
			{
				return _propertyName;
			}

			inline
			ContainerPtr
			container() const
			{
				return _container;
			}

			inline
			bool
			operator==(const ContainerProperty& x) const
			{
				return _propertyName == x._propertyName && _container == x._container;
			}

			ContainerProperty&
			initialize(const data::MacroBinding&, ContainerPtr targetData, ContainerPtr rendererData, ContainerPtr rootData);

			/*
			ContainerProperty&
			initialize(const std::string&, ContainerPtr targetData, ContainerPtr rendererData, ContainerPtr rootData);
			*/
		};
	}
}

namespace std
{
	template<> struct hash<minko::data::ContainerProperty>
	{
		inline
		size_t
		operator()(const minko::data::ContainerProperty& x)const
		{
			size_t seed = std::hash<std::shared_ptr<minko::data::Container>>()(x.container());

			hash_combine<std::string>(seed, x.name());

			return seed;
		}
	};
}