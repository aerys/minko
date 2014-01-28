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

#include "AssetsDeserializer.hpp"
#include "minko/deserialize/MkTypes.hpp"
#include "minko/Qark.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/Options.hpp"
#include "minko/material/Material.hpp"

using namespace minko;

namespace minko
{
	namespace deserialize
	{
		AssetsDeserializer::AssetsDeserializer(Any					    			assetsData,
											   std::shared_ptr<file::AssetLibrary>	library) :
			_assetsData(assetsData),
			_library(library)
		{
		}

		void
		AssetsDeserializer::extract(std::shared_ptr<file::Options>				options,
									std::shared_ptr<deserialize::NameConverter> nameConverter)
		{
			std::vector<Any>	assetList;
			try
			{
				std::map<std::string, Any>&	assetData = Any::cast<std::map<std::string, Any>&>(_assetsData);
				assetList = Any::cast<std::vector<Any>&>(assetData["assets"]);
			}
			catch (...)
			{
				assetList = Any::cast<std::vector<Any>&>(_assetsData);
			}

			for (unsigned int assetIndex = 0; assetIndex < assetList.size(); ++assetIndex)
			{
				Any& assetBlob							= assetList[assetIndex];
				std::map<std::string, Any>& assetData	= Any::cast<std::map<std::string, Any>&>(assetBlob);

				int&			assetType	= Any::cast<int&>(assetData["type"]);
				std::string&	assetName	= Any::cast<std::string&>(assetData["name"]);
				int&			assetId		= Any::cast<int&>(assetData["id"]);

				if (assetType == MkTypes::TEXTURE_ASSET)
				{
					Qark::ByteArray&						textureData		= Any::cast<Qark::ByteArray&>(assetData["data"]);
					std::shared_ptr<file::AbstractParser>	pngParser		= _library->parser("png");
					std::vector<unsigned char>&				uTextureData	= reinterpret_cast<std::vector<unsigned char>&>(*&textureData);

					pngParser->parse(
						assetName, 
						assetName, 
						options, 
						uTextureData, 
						_library
					);

					_idToTexture[assetId] = _library->texture(assetName);
				}
				else if (assetType == MkTypes::MATERIAL_ASSET)
				{					
					std::vector<Any>& properties = Any::cast<std::vector<Any>&>(assetData["bindings"]);

					std::shared_ptr<data::Provider> material = TypeDeserializer::provider(
						options->material(), 
						properties, 
						_idToTexture, 
						nameConverter
					);

					_idToMaterial[assetId]	= options->materialFunction()(
						assetName, 
						std::static_pointer_cast<material::Material>(material)
					);
				}
			}
		}

	}
}