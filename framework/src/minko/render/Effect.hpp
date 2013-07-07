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
			typedef std::shared_ptr<Effect>		Ptr;

		private:
			typedef std::shared_ptr<Pass>		PassPtr;

		private:
			std::vector<PassPtr>							_passes;

			std::unordered_map<std::string, std::string>	_attributeBindings;
			std::unordered_map<std::string, std::string>	_uniformBindings;
			std::unordered_map<std::string, std::string>	_stateBindings;

		public:
			inline static
			Ptr
			create(std::vector<PassPtr>&						passes,
				   std::unordered_map<std::string, std::string>&	attributeBindings,
				   std::unordered_map<std::string, std::string>&	uniformBindings,
				   std::unordered_map<std::string, std::string>&	stateBindings)
			{
				return std::shared_ptr<Effect>(new Effect(
					passes, attributeBindings, uniformBindings, stateBindings
				));
			}

			inline
			const std::vector<PassPtr>&
			passes()
			{
				return _passes;
			}

			inline
			const std::unordered_map<std::string, std::string>&
			attributeBindings()
			{
				return _attributeBindings;
			}

			inline
			const std::unordered_map<std::string, std::string>&
			uniformBindings()
			{
				return _uniformBindings;
			}

			inline
			const std::unordered_map<std::string, std::string>&
			stateBindings()
			{
				return _stateBindings;
			}

		private:
			Effect(std::vector<PassPtr>&						passes,
				   std::unordered_map<std::string, std::string>&	attributeBindings,
				   std::unordered_map<std::string, std::string>&	uniformBindings,
				   std::unordered_map<std::string, std::string>&	stateBindings);
		};		
	}
}
