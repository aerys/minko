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

#include "minko/file/AbstractWriter.hpp"

namespace minko
{
	namespace file
	{
		class TextureWriter :
            public AbstractWriter<std::shared_ptr<render::AbstractTexture>>
		{
		public:
			typedef std::shared_ptr<TextureWriter>										Ptr;
			typedef msgpack::type::tuple<std::string, uint, uint, std::vector<uint>>	SerializedNode;

		private:
			typedef std::shared_ptr<file::Dependency> 					DependencyPtr;
			typedef std::shared_ptr<file::AssetLibrary>					AssetLibraryPtr;
			typedef std::shared_ptr<Options>							OptionsPtr;

        private:
            std::string _extension;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<TextureWriter>(new TextureWriter());
			}

            void
            extension(const std::string& extension);

            void
            writeRawTexture(std::string&					filename,
                            std::shared_ptr<AssetLibrary>	assetLibrary,
                            std::shared_ptr<Options>		options,
                            std::shared_ptr<WriterOptions>  writerOptions);

			std::string
			embed(AssetLibraryPtr                       assetLibrary,
                  OptionsPtr                            options,
                  DependencyPtr                         dependency,
                  std::shared_ptr<WriterOptions>        writerOptions);

		protected:
			TextureWriter();
		};
	}
}

