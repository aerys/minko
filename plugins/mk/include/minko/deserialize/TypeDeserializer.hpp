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

#include "minko/MkCommon.hpp"
#include "minko/Qark.hpp"

namespace minko
{
	namespace deserialize
	{
		class TypeDeserializer
		{

		// typedef
		public :
			typedef	std::function<void(
				std::map<int, std::shared_ptr<render::AbstractTexture>>&, 
				std::string&, 
				std::shared_ptr<data::Provider>, 
				Qark::Map&, 
				std::shared_ptr<NameConverter>)> ReadMkTypeFunction;


		// property
		private:
			static std::map<const unsigned int, ReadMkTypeFunction> _typeToReadFunc;

		// methods
		public:
			
			static
			std::shared_ptr<math::Matrix4x4>
			matrix4x4(Any& matrixObject);

			static
			std::shared_ptr<data::Provider>
			provider(std::shared_ptr<data::Provider>							defaultValues,
					 std::vector<Any>&											properties,
					 std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
					 std::shared_ptr<NameConverter>								nameConverter);

			static
			void
			texture(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
					std::string&												propertyName,
					std::shared_ptr<data::Provider>								material,
					Qark::Map&													object,
					std::shared_ptr<NameConverter>								nameConverter);

			static
			void
			boolean(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
				    std::string&												propertyName,
				    std::shared_ptr<data::Provider>								material,
				    Qark::Map&													object,
				    std::shared_ptr<NameConverter>								nameConverter);

			static
			void
			number(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
				   std::string&												propertyName,
				   std::shared_ptr<data::Provider>							material,
				   Qark::Map&												object,
				   std::shared_ptr<NameConverter>							nameConverter);

			static
			void
			vector4(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
				    std::string&												propertyName,
				    std::shared_ptr<data::Provider>								material,
				    Qark::Map&													object,
				    std::shared_ptr<NameConverter>								nameConverter);

						static
			void
			vector4b(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
				     std::string&												propertyName,
				     std::shared_ptr<data::Provider>							material,
				     Qark::Map&													object,
				     std::shared_ptr<NameConverter>								nameConverter);

			void
			unsupport(std::map<int, std::shared_ptr<render::AbstractTexture>>&	idToTexture,
				      std::string&												propertyName,
				      std::shared_ptr<data::Provider>							material,
				      Qark::Map&												object,
				      std::shared_ptr<NameConverter>							nameConverter);

		private:
			static
			std::map<const unsigned int, ReadMkTypeFunction>
			initIdToFunctionMap();
		};
	}
}
