/*
Copyright (c) 2014 Aerys

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
#include "minko/file/AbstractSerializerParser.hpp"
#include "msgpack.hpp"

namespace minko
{
	namespace file
	{
		class MaterialParser :
			public AbstractSerializerParser
		{
		private:
			typedef std::shared_ptr<MaterialParser>								Ptr;

            typedef msgpack::type::tuple<uint, std::string>                     ComplexPropertyValue;
			typedef msgpack::type::tuple<std::string, std::string>				BasicProperty;
			typedef msgpack::type::tuple<std::string, ComplexPropertyValue>		ComplexProperty;
			typedef std::shared_ptr<material::Material>							MaterialPtr;
			typedef std::shared_ptr<AssetLibrary>								AssetLibraryPtr;
			typedef std::shared_ptr<Options>									OptionsPtr;
			typedef std::shared_ptr<render::AbstractTexture>					TexturePtr;

		private:
			static std::map<uint, std::function<Any(std::tuple<uint, std::string&>&)>> _typeIdToReadFunction;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<MaterialParser>(new MaterialParser());
			}

			void
			parse(const std::string&				filename,
				  const std::string&                resolvedFilename,
				  OptionsPtr						options,
				  const std::vector<unsigned char>&	data,
				  AssetLibraryPtr					assetLibrary);

		private:
			void
			deserializeComplexProperty(MaterialPtr			material,
									   ComplexProperty		serializedProperty);

			void
			deserializeBasicProperty(MaterialPtr		material,
									 BasicProperty		serializedProperty);

			MaterialParser();
		};
	}
}
