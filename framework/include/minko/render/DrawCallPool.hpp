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

#include "minko/Signal.hpp"

namespace std
{
	template<>
	struct hash< std::pair<std::shared_ptr<minko::component::Surface>, std::shared_ptr<minko::scene::Node>>>
	{
		inline
		size_t
		operator()(const std::pair<std::shared_ptr<minko::component::Surface>, std::shared_ptr<minko::scene::Node>>& x) const
		{
			size_t seed = std::hash<long>()(long(x.first.get()));

			hash_combine(seed, std::hash<long>()(long(x.second.get())));

			return seed;
		}
	};
}

namespace minko
{
	namespace render
	{

		class DrawCallPool :
			public std::enable_shared_from_this<DrawCallPool>
		{
		public:
			typedef std::shared_ptr<DrawCallPool>														Ptr;

		private:
            std::list<std::shared_ptr<DrawCall>> _drawCalls;

		public:
			inline static
			Ptr
			create()
			{
				Ptr ptr = std::shared_ptr<DrawCallPool>(new DrawCallPool());

				return ptr;
			}

			const std::list<std::shared_ptr<DrawCall>>&
			drawCalls();
			
		private:
			explicit
			DrawCallPool();

			void
			initialize();
		};
	}
}

