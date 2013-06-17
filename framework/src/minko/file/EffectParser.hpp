#pragma once

#include "minko/Common.hpp"

namespace
{
	using namespace minko::render::context;
	using namespace minko::render;
}

namespace minko
{
	namespace file
	{
		class EffectParser
		{
		private:
			std::shared_ptr<Effect>	_effect;

		public:
			inline
			std::shared_ptr<Effect>
			effect()
			{
				return _effect;
			}

			void
			parse(std::shared_ptr<AbstractContext> context, const std::string& data);
		};
	}
}
