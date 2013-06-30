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
#include "minko/file/AbstractParser.hpp"

namespace
{
	using namespace minko::render;
	using namespace minko::render;
}

namespace minko
{
	namespace file
	{
		class EffectParser :
			public std::enable_shared_from_this<EffectParser>,
			public AbstractParser
		{
		public:
			typedef std::shared_ptr<EffectParser>	Ptr;

		private:
			typedef std::shared_ptr<Loader> LoaderPtr;

		private:
			std::shared_ptr<Effect>									_effect;
			std::string												_effectName;
			
			unsigned int											_numDependencies;
			unsigned int											_numLoadedDependencies;

			std::shared_ptr<AbstractContext>						_context;
			std::vector<std::pair<std::string, std::string>>		_programs;
			std::unordered_map<std::string, std::string>			_bindings;
			std::string												_dependenciesCode;

			std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>	_loaderCompleteSlots;
			std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>	_loaderErrorSlots;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<EffectParser>(new EffectParser());
			}

			inline
			std::shared_ptr<Effect>
			effect()
			{
				return _effect;
			}

			inline
			const std::string&
			effectName()
			{
				return _effectName;
			}

			void
			parse(const std::string&		filename,
				  std::shared_ptr<Options>	options,
				  const std::vector<char>&	data);

			void
			dependencyCompleteHandler(std::shared_ptr<Loader> loader);

			void
			dependencyErrorHandler(std::shared_ptr<Loader> loader);

			void
			finalize();

		private:
			EffectParser();
		};
	}
}
