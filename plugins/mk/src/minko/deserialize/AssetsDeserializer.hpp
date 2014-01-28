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
#include "minko/Any.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/deserialize/NameConverter.hpp"

namespace minko
{
	namespace deserialize
	{
		class AssetsDeserializer
		{
		public:
			typedef std::shared_ptr<AssetsDeserializer> Ptr;

		private:
			typedef std::map<int, std::shared_ptr<render::AbstractTexture>>		TextureMap;
			typedef std::map<int, std::shared_ptr<data::Provider>>				MaterialMap;

		private:
			TextureMap							_idToTexture;
			MaterialMap							_idToMaterial;
			Any									_assetsData;
			std::shared_ptr<file::AssetLibrary>	_library;

		public:
			inline static
			Ptr
			create(Any					    			assetsData, 
				   std::shared_ptr<file::AssetLibrary>	library)
			{
				return std::shared_ptr<AssetsDeserializer>(new AssetsDeserializer(assetsData, library));
			}

			inline
			std::shared_ptr<data::Provider>
			material(int id)
			{
				return _idToMaterial[id];
			}

			inline
			std::shared_ptr<render::AbstractTexture>
			texture(int id)
			{
				return _idToTexture[id];
			}

			void
			extract(std::shared_ptr<file::Options>				options,
					std::shared_ptr<deserialize::NameConverter> nameConverter);
		private:
			AssetsDeserializer(Any					    			assetsData,
							   std::shared_ptr<file::AssetLibrary>	library);
		};
	}
}
