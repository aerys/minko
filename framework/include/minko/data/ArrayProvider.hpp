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
		class ArrayProvider :
			public Provider
		{
		public:
			typedef std::shared_ptr<ArrayProvider>			Ptr;

		private:
			typedef std::shared_ptr<Provider>				ProviderPtr;
			typedef Signal<Ptr, uint>						IndexChangedSignal;
			typedef std::shared_ptr<IndexChangedSignal>		IndexChangedSignalPtr;

		private:
			std::string										_name;
			IndexChangedSignalPtr							_indexChanged;

		public:
			inline static
			Ptr
			create(const std::string& name)
			{
				return std::shared_ptr<ArrayProvider>(new ArrayProvider(name));
			}


			inline
			IndexChangedSignalPtr
			indexChanged()
			{
				return _indexChanged;
			}

			inline
			const std::string&
			arrayName() const
			{
				return _name;
			}

			inline
			Ptr
			copyFrom(Ptr source)
			{
				Provider::copyFrom(std::static_pointer_cast<Provider>(source));

				_name = source->_name;

				return std::static_pointer_cast<ArrayProvider>(shared_from_this());
			}

			inline
			Ptr
			clone()
			{
				auto that = std::static_pointer_cast<ArrayProvider>(shared_from_this());

				return ArrayProvider::create(_name)->copyFrom(that);
			}

		protected:
			explicit
			ArrayProvider(const std::string& name);
	
		};
	}
}
