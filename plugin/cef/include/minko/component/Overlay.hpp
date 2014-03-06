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
#include "minko/Signal.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/dom/AbstractDOMElement.hpp";
#include "minko/dom/AbstractDOMEngine.hpp";

namespace minko
{
	namespace component
	{

		class Overlay :
			AbstractComponent
		{
		public:
			
			void
			load(std::string uri)
			{
				return _domEngine->load(uri);
			}

			void
			unload()
			{
				_domEngine->unload();
			}

			std::shared_ptr<dom::AbstractDOMElement>
			document()
			{
				return _document;
			}

			std::shared_ptr<Signal<std::string>>
			onLoad()
			{
				return _onLoad;
			}
			
		private:
			std::shared_ptr<dom::AbstractDOMEngine> _domEngine;
			std::shared_ptr<dom::AbstractDOMElement> _document;
			std::shared_ptr<Signal<std::string>> _onLoad;
		};
	}
}