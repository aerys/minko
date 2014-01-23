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
		class Texture :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<Texture> Ptr;

			enum DataFormat
			{
				RGB,
				RGBA
			};

		public:
			static const uint MAX_SIZE;

		private:
			unsigned int				_width;		
			unsigned int				_height;	
			unsigned int				_widthGPU;	// always power of 2
			unsigned int				_heightGPU;	// always power of 2
            bool                        _mipMapping;
            bool                        _optimizeForRenderToTexture;
			const bool					_resizeSmoothly;
			std::vector<unsigned char>	_data;
			std::string					_filename;

		public:
			~Texture()
			{
				dispose();
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context,
				   const unsigned int						width,
				   const unsigned int						height,
                   bool                                     mipMapping                  = false,
                   bool                                     optimizeForRenderToTexture  = false,
				   bool										resizeSmoothly				= true,
				   std::string								filename					= "")
			{
				return std::shared_ptr<Texture>(new Texture(context, width, height, mipMapping, optimizeForRenderToTexture, resizeSmoothly, filename));
			}

			inline
			const unsigned int
			width()
			{
				return _width;
			}

			inline
			const unsigned int
			height()
			{
				return _height;
			}

            inline
            const bool
            mipMapping()
            {
                return _mipMapping;
            }

			void
			data(unsigned char* data, 
				 DataFormat		format		= DataFormat::RGBA, 
				 int			widthGPU	= -1, 
				 int			heightGPU	= -1);

			inline
			std::vector<unsigned char>&
			data()
			{
				return _data;
			}

			void
			dispose();

			void
			upload();

			void
			uploadMipLevel(uint							miplevel,
						   std::vector<unsigned char>	data,
						   bool							optimizeForRenderToTexture);


		private:
			Texture(std::shared_ptr<render::AbstractContext>	context,
					const unsigned int							width,
					const unsigned int							height,
                    bool                                        mipMapping,
                    bool                                        optimizeForRenderToTexture,
					bool										resizeSmoothly,
				   std::string									filename					= "");

			void
			processData(std::vector<unsigned char>& inData, std::vector<unsigned char>& outData) const;
		};
	}
}
