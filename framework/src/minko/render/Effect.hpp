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
			typedef std::shared_ptr<resource::Program>		ProgramPtr;

		private:
			std::vector<ProgramPtr>							_shaders;
			std::unordered_map<std::string, std::string>	_attributeBindings;
			std::unordered_map<std::string, std::string>	_uniformBindings;
			std::unordered_map<std::string, std::string>	_stateBindings;

			std::shared_ptr<data::Provider>					_data;

		public:
			inline static
			Ptr
			create(std::vector<ProgramPtr>							shaders,
				   std::unordered_map<std::string, std::string>&	attributeBindings,
				   std::unordered_map<std::string, std::string>&	uniformBindings,
				   std::unordered_map<std::string, std::string>&	stateBindings)
			{
				return std::shared_ptr<Effect>(new Effect(
					shaders, attributeBindings, uniformBindings, stateBindings
				));
			}

			inline
			std::shared_ptr<data::Provider>
			data()
			{
				return _data;
			}

			inline
			const std::vector<ProgramPtr>&
			shaders()
			{
				return _shaders;
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
			Effect(std::vector<ProgramPtr>&							shaders,
				   std::unordered_map<std::string, std::string>&	attributeBindings,
				   std::unordered_map<std::string, std::string>&	uniformBindings,
				   std::unordered_map<std::string, std::string>&	stateBindings);

			void
			propertyChangedHandler(std::shared_ptr<data::Container> data,
								   const std::string&				propertyName);
		};		
	}
}
