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
#include "msgpack.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/file/Options.hpp"

namespace minko
{
	namespace serializer
	{
		class MkStats
		{
		public:
			typedef std::shared_ptr<MkStats> Ptr;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<MkStats>(new MkStats());
			}

			template <typename T, typename SerializedT>
			void
			compressionStat(T								value, 
							std::function<SerializedT(T)>	f1, 
							std::function<Any(SerializedT)>	f2,
							std::string						text)
			{
				SerializedT			res = f1(value);
				std::stringstream	buffer;

				msgpack::pack(buffer, res);

				std::string			str(buffer.str());
				msgpack::zone		mempool;
				msgpack::object		deserialized;

				msgpack::unpack(str.data(), str.size(), NULL, &mempool, &deserialized);

				SerializedT dst;
			
				deserialized.convert(&dst);

				T deserializedValue = Any::cast<T>(f2(dst));

				std::cout << text << std::endl;
				std::cout << "	Size " << buffer.str().size() << std::endl;
				std::cout << "	Valid " << (deserializedValue == value) << std::endl << std::endl;
			}

			template <typename AssetT, typename ParserT, typename WriterT>
			void
			assetStat(render::AbstractContext::Ptr			context,
					  std::shared_ptr<file::AssetLibrary>	assets,
					  std::string 							assetName, 
					  AssetT								asset)
			{
				std::shared_ptr<WriterT>	writer			= WriterT::create();
				
				writer->data(asset);
				writer->write("assetStat.tmp", assets, nullptr);

				std::shared_ptr<file::Options> options = file::Options::create(context);
				std::vector<unsigned char>      _data;
				auto flags = std::ios::in | std::ios::ate | std::ios::binary;
				std::fstream file("assetStat.tmp", flags);

				unsigned int size = (unsigned int)file.tellg();
				_data.resize(size);

				file.seekg(0, std::ios::beg);
				file.read((char*)&_data[0], size);
				file.close();
				
				std::shared_ptr<ParserT> parser = ParserT::create();

				parser->parse("assetStat.tmp", "assetStat.tmp", options, _data, assets);

				std::cout << assetName << std::endl;
				std::cout << "	Size : " << _data.size() << std::endl;
				printValidation(asset, assetName, assets);
				std::cout << std::endl;
			}

		private:
			template <typename AssetT>
			void
			printValidation(AssetT originalAsset, std::string assetName, file::AssetLibrary::Ptr assets)
			{
				std::cout << "	Valid : " << "can't compare" << std::endl;
			}

			void
			printValidation(std::shared_ptr<geometry::Geometry> originalAsset, std::string assetName, file::AssetLibrary::Ptr assets)
			{
				std::cout << "	Valid : " << originalAsset->equals(assets->geometry(assetName)) << std::endl;
			}

			void
				printValidation(std::shared_ptr<material::Material> materialAsset, std::string assetName, file::AssetLibrary::Ptr assets)
			{
				std::cout << "	Valid : ??????" << std::endl;
			}

			MkStats()
			{
			}
		};
	}
}
