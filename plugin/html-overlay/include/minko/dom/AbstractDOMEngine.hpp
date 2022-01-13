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
#include "AbstractDOM.hpp"

namespace minko
{
	namespace dom
	{
		class AbstractDOMEngine
		{
		public:
			typedef std::shared_ptr<AbstractDOMEngine> Ptr;
			
			virtual
			~AbstractDOMEngine()
			{
			}
 
			virtual
			AbstractDOM::Ptr
			load(std::string uri) = 0;

			virtual
			void
			clear() = 0;

			virtual
			Signal<AbstractDOM::Ptr, std::string>::Ptr
			onload() = 0;

			virtual
			Signal<AbstractDOM::Ptr, std::string>::Ptr
			onmessage() = 0;

			virtual
			AbstractDOM::Ptr
			mainDOM() = 0;

			virtual
			void
			visible(bool) = 0;
            
            virtual
			bool
			visible() = 0;

            virtual
            void
            updateNextFrame() = 0;

            virtual
            void
            pollRate(int) = 0;

            // Render to texture
            
            virtual
            void
            enableRenderToTexture(std::shared_ptr<minko::render::AbstractTexture> texture) = 0;

            virtual
            void
            disableRenderToTexture() = 0;

            // Set the origins allowed for the overlays.
            virtual
			void
            setOverlayAllowedOrigins(const std::vector<std::string>& allowedOrigins)
            {
            }
		};
	}
}