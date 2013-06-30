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
		class DrawCall
		{
		public:
			typedef std::shared_ptr<DrawCall> Ptr;

		private:
			typedef std::shared_ptr<AbstractContext>	AbsCtxPtr;

		private:
			std::shared_ptr<data::Container>					_data;
			const std::unordered_map<std::string, std::string>&	_inputNameToBindingName;

			std::vector<std::function<void(AbsCtxPtr)>>			_func;

		public:
			static inline
			Ptr
			create(std::shared_ptr<data::Container>						bindings,
				   const std::unordered_map<std::string, std::string>&	inputNameToBindingName)
			{
				return std::shared_ptr<DrawCall>(new DrawCall(bindings, inputNameToBindingName));
			}

			void
			render(std::shared_ptr<AbstractContext> context);

			void
			initialize(std::shared_ptr<data::Container>				bindings,
					   const std::map<std::string, std::string>&	inputNameToBindingName);

		private:
			DrawCall(std::shared_ptr<data::Container>						bindings,
					 const std::unordered_map<std::string, std::string>&	inputNameToBindingName);

			void
			bind(std::shared_ptr<data::Container> bindings);
		};		
	}
}
