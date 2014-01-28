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

#include "minko/render/AbstractResource.hpp"

namespace minko
{
	namespace render
	{
		class AbstractTexture :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<AbstractTexture>	Ptr;

		private:
			typedef std::shared_ptr<AbstractContext>	AbstractContextPtr;

		public:
			static const uint MAX_SIZE;

		protected:
			const TextureType	_type;
			unsigned int		_width;		
			unsigned int		_height;	
			unsigned int		_widthGPU;	// always power of 2
			unsigned int		_heightGPU;	// always power of 2
            bool				_mipMapping;
			bool				_resizeSmoothly;
			bool				_optimizeForRenderToTexture;
			std::string			_filename;			

		public:
			inline
			TextureType
			type() const
			{
				return _type;
			}

			inline
			uint
			width() const
			{
				return _widthGPU;
			}

			inline
			uint
			height() const
			{
				return _heightGPU;
			}

            inline
            bool
            mipMapping() const
            {
                return _mipMapping;
            }

			inline
			bool
			optimizeForRenderToTexture() const
			{
				return _optimizeForRenderToTexture;
			}

			virtual
			void
			data(unsigned char*, 
				 TextureFormat	format		= TextureFormat::RGBA,
				 int			widthGPU	= -1,
				 int			heightGPU	= -1) = 0;

			//virtual
			//void
			//uploadMipLevel(uint	level,
			//			   unsigned char*) = 0;

			virtual
			void
			disposeData() = 0;

		protected:
			AbstractTexture(TextureType			type,
							AbstractContextPtr	context,
							unsigned int		width,
							unsigned int		height,
							bool                mipMapping,
							bool				optimizeForRenderToTexture,
							bool				resizeSmoothly,
							const std::string&	filename);

			~AbstractTexture()
			{
			}

			static
			void
			resizeData(unsigned int width, 
					   unsigned int height, 
					   std::vector<unsigned char>&	data, 
			           unsigned int newWidth, 
					   unsigned int newHeight,
					   bool resizeSmoothly,
					   std::vector<unsigned char>&	newData);

			uint
			getMipmapWidth(uint level) const;

			uint
			getMipmapHeight(uint level) const;
		};
	}
}
