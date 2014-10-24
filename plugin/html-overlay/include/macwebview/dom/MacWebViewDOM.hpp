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
#include "minko/component/SceneManager.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "MacWebViewDOMElement.hpp"
#include "minko/dom/AbstractDOM.hpp"

#import "WebViewJavascriptBridge.h"

namespace macwebview
{
	namespace dom
	{
        class MacWebViewDOMEngine;
        
		class MacWebViewDOM : public minko::dom::AbstractDOM,
            public std::enable_shared_from_this<MacWebViewDOM>
		{
		public:
			typedef std::shared_ptr<MacWebViewDOM> Ptr;

		private:
			MacWebViewDOM(const std::string&);

		public:
			static
			Ptr
			create(const std::string&, std::shared_ptr<MacWebViewDOMEngine> engine);

			void
			sendMessage(const std::string&, bool async);

			void
			eval(const std::string&, bool async);

			minko::dom::AbstractDOMElement::Ptr
			createElement(const std::string&);

			minko::dom::AbstractDOMElement::Ptr
			getElementById(const std::string&);

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			getElementsByClassName(const std::string&);

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			getElementsByTagName(const std::string&);

			minko::dom::AbstractDOMElement::Ptr
			document();

			minko::dom::AbstractDOMElement::Ptr
			body();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onload();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onmessage();

			std::string
			fileName();

			std::string
			fullUrl();

			bool
			isMain();

			bool
			initialized();

			void
			initialized(bool);

			std::vector<minko::dom::AbstractDOMElement::Ptr>
			getElementList(const std::string&);
            
            void
            runScript(const std::string& script);
            
            std::string
            runScriptString(const std::string& script);
            
            int
            runScriptInt(const std::string& script);

		private:
			bool _initialized;

			std::string _jsAccessor;
            
            std::shared_ptr<MacWebViewDOMEngine> _engine;
            
			MacWebViewDOMElement::Ptr _document;
			MacWebViewDOMElement::Ptr _body;
            
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onload;
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onmessage;
		};
	}
}