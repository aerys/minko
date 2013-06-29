#pragma once

#include "minko/Common.hpp"

namespace
{
	using namespace minko::render::context;
}

namespace minko
{
	namespace file
	{
		class Options
		{
		public:
			typedef std::shared_ptr<Options> Ptr;

		private:
			std::shared_ptr<AbstractContext>	_context;
			std::string							_includePath;

		public:
			inline static
			Ptr
			create(std::shared_ptr<AbstractContext> context)
			{
				return std::shared_ptr<Options>(new Options(context));
			}

			inline
			std::shared_ptr<AbstractContext>
			context()
			{
				return _context;
			}
			
			inline
			const std::string&
			includePath()
			{
				return _includePath;
			}

			inline
			void
			includePath(const std::string& includePath)
			{
				_includePath = includePath;
			}

		private:
			Options(std::shared_ptr<AbstractContext> context) :
				_context(context)
			{
			}
		};
	}
}
