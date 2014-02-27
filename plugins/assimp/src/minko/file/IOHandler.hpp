/*
Copyright(c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/Common.hpp"

#include "minko/file/Options.hpp"
#include "minko/file/IOStream.hpp"

#include "assimp/IOStream.hpp"
#include "assimp/IOSystem.hpp"

namespace minko
{
	namespace file
	{
		class IOHandler :
			public Assimp::IOSystem
		{
			typedef std::shared_ptr<AbstractLoader>					LoaderPtr;

			typedef Signal<LoaderPtr>::Slot							LoaderSignalSlot;

			typedef std::unordered_map<LoaderPtr, LoaderSignalSlot>	LoaderToSlotMap;
			typedef std::unordered_map<uint, std::string>			TextureTypeToName;
		private:
			std::shared_ptr<file::Options>		_options;
			std::shared_ptr<file::AssetLibrary>	_assets;
			LoaderToSlotMap											_loaderCompleteSlots;
			LoaderToSlotMap											_loaderErrorSlots;

		public:
			IOHandler(std::shared_ptr<file::Options> options, std::shared_ptr<file::AssetLibrary> assets) :
				_options(options),
				_assets(assets)
			{

			}

			void
			Close(Assimp::IOStream* pFile)
			{

			}

			bool
			Exists(const char*  pFile) const
			{
				std::ifstream f(pFile);

				return (bool)f;
			}

			char
			getOsSeparator() const
			{
#ifdef _WIN32
				return '\\';
#else
				return '/';
#endif
			}

			Assimp::IOStream*
			Open(const char* pFile, const char* pMode = "rb")
			{
				auto filename = std::string(pFile);
				auto loader = _options->loaderFunction()(filename, _assets);
				
				Assimp::IOStream* stream = 0;

				_loaderCompleteSlots[loader] = loader->complete()->connect([&](file::AbstractLoader::Ptr loader)
				{
					stream = new minko::file::IOStream(loader->data());
				});
#ifdef DEBUG
				_loaderErrorSlots[loader] = loader->error()->connect([&](file::AbstractLoader::Ptr loader)
				{
					std::cerr << "error: could not load file '" << filename << "'" << std::endl;
				});
#endif

				loader->load(filename, _options);
				
				return stream;
			}
		};
	}
}