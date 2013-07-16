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
	namespace render
	{

		class Effect :
			public std::enable_shared_from_this<Effect>
		{
		public:
			typedef std::shared_ptr<Effect>	Ptr;

		private:
			typedef std::shared_ptr<Pass>	PassPtr;

		private:
			std::vector<PassPtr>						_passes;
			std::list<std::shared_ptr<EffectInstance>>	_instances;

		public:
			inline static
			Ptr
			create(std::vector<PassPtr>&	passes)
			{
				return std::shared_ptr<Effect>(new Effect(passes));
			}

			inline
			const std::vector<PassPtr>&
			passes()
			{
				return _passes;
			}

			std::shared_ptr<EffectInstance>
			instanciate(std::shared_ptr<data::Container> data);

		private:
			Effect(std::vector<PassPtr>&	passes);
		};		
	}
}
