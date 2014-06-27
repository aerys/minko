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
#include "minko/Types.hpp"
#include "minko/SerializerCommon.hpp"

namespace minko
{
	namespace file
	{
		class TextureWriter
		{
		public:
			typedef std::shared_ptr<TextureWriter>										Ptr;

		private:
			typedef std::shared_ptr<AssetLibrary>					AssetLibraryPtr;
			typedef std::shared_ptr<Options>							OptionsPtr;
			typedef std::shared_ptr<WriterOptions>					    WriterOptionsPtr;

            typedef std::shared_ptr<render::Texture>                    TexturePtr;

        private:
            serialize::ImageFormat _imageFormat;
            TexturePtr _data;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<TextureWriter>(new TextureWriter());
			}

            void
            data(TexturePtr data);

            void
            imageFormat(serialize::ImageFormat imageFormat);

            void
            writeRawTexture(std::string&        filename,
                            AssetLibraryPtr     assetLibrary,
                            OptionsPtr		    options,
                            WriterOptionsPtr    writerOptions);

			std::string
			embedTexture(AssetLibraryPtr    assetLibrary,
                         OptionsPtr         options,
                         WriterOptionsPtr   writerOptions);

		protected:
			TextureWriter();
		};
	}
}

