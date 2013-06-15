#pragma once

#include "minko/Common.hpp"

namespace
{
	using namespace minko::scene;
	using namespace minko::scene::data;
}

namespace minko
{
	namespace render
	{

		class Effect :
			public std::enable_shared_from_this<Effect>
		{
		public:
			typedef std::shared_ptr<Effect>	ptr;

		private:
			typedef std::shared_ptr<GLSLProgram>		GLSLProgramPtr;

		private:
			std::vector<GLSLProgramPtr>						_shaders;
			std::shared_ptr<DataProvider>					_data;
			std::unordered_map<std::string, std::string>	_inputNameToBindingName;

		public:
			inline static
			ptr
			create(std::vector<GLSLProgramPtr> shaders)
			{
				return std::shared_ptr<Effect>(new Effect(shaders));
			}

			inline
			std::shared_ptr<DataProvider>
			data()
			{
				return _data;
			}

			inline
			const std::vector<GLSLProgramPtr>&
			shaders()
			{
				return _shaders;
			}

			inline
			const std::unordered_map<std::string, std::string>&
			inputNameToBindingName()
			{
				return _inputNameToBindingName;
			}

			ptr
			bindInput(const std::string& bindingName, const std::string& programInputName);

		private:
			Effect(std::vector<GLSLProgramPtr> shaders);

			void
			propertyChangedHandler(std::shared_ptr<DataBindings> bindings, const std::string& propertyName);
		};		
	}
}
