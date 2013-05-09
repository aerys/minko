#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace render
	{
		using namespace minko::scene;

		class Effect :
			public std::enable_shared_from_this<Effect>
		{
		public:
			typedef std::shared_ptr<Effect>	ptr;

		private:
			typedef std::shared_ptr<data::DataBindings>	DataBindingsPtr;
			typedef std::shared_ptr<GLSLProgram>		GLSLProgramPtr;

		private:
			DataBindingsPtr 			_bindings;
			std::vector<GLSLProgramPtr>	_shaders;

		public:
			inline static
			ptr
			create(DataBindingsPtr bindings, std::vector<GLSLProgramPtr> shaders)
			{
				return std::shared_ptr<Effect>(new Effect(bindings, shaders));
			}

			ptr
			bind(const std::string& bindingName, const std::string& propertyName);

		private:
			Effect(DataBindingsPtr bindings, std::vector<GLSLProgramPtr> shaders);

			void
			propertyChangedHandler(DataBindingsPtr bindings, const std::string& propertyName);
		};		
	}
}
