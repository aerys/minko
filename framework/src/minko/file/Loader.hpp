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

namespace
{
}

namespace minko
{
	namespace file
	{
		class Loader :
			public std::enable_shared_from_this<Loader>
		{
		public:
			typedef std::shared_ptr<Loader>	Ptr;

		private:
			std::vector<char>				_data;

			std::shared_ptr<Options>		_options;

			std::shared_ptr<Signal<Ptr>>	_complete;
			std::shared_ptr<Signal<Ptr>>	_error;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Loader>(new Loader());
			}

			inline
			const std::vector<char>&
			data()
			{
				return _data;
			}

			inline
			std::shared_ptr<Options>
			options()
			{
				return _options;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			complete()
			{
				return _complete;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			error()
			{
				return _error;
			}

			void
			load(const std::string& filename, std::shared_ptr<Options> options);

		private:
			Loader();
		};
	}
}
