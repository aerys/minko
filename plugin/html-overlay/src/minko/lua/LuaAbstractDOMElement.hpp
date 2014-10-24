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

#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "minko/dom/AbstractDOMMouseEvent.hpp"
#include "minko/dom/AbstractDOMTouchEvent.hpp"
#include "minko/MinkoLua.hpp"

namespace minko
{
	class LuaWrapper;

	namespace component
	{
		namespace overlay
		{
			class LuaAbstractDOMElement :
				public LuaWrapper
			{

			private:

			public:

				static
					void
					bind(LuaGlue& state)
				{
						state.Class<std::vector<dom::AbstractDOMElement::Ptr>>("std__vector_dom__AbstractDOMElement__Ptr_")
							.methodWrapper("at", &LuaAbstractDOMElement::atWrapper)
							.property("size", &std::vector<dom::AbstractDOMElement::Ptr>::size);
						//.index(&LuaNode::getWrapper);

						auto& abstractDomElement = state.Class<dom::AbstractDOMElement>("AbstractDOMElement")
							.property("tagName", &dom::AbstractDOMElement::tagName)
							.property("parentNode", &dom::AbstractDOMElement::parentNode)
							.method("getId", static_cast<std::string(dom::AbstractDOMElement::*)(void)>(&dom::AbstractDOMElement::id))
                            .method("setId", static_cast<void(dom::AbstractDOMElement::*)(const std::string&)>(&dom::AbstractDOMElement::id))
							.method("getClassName", static_cast<std::string(dom::AbstractDOMElement::*)(void)>(&dom::AbstractDOMElement::className))
                            .method("setClassName", static_cast<void(dom::AbstractDOMElement::*)(const std::string&)>(&dom::AbstractDOMElement::className))
							.method("getTextContent", static_cast<std::string(dom::AbstractDOMElement::*)(void)>(&dom::AbstractDOMElement::textContent))
                            .method("setTextContent", static_cast<void(dom::AbstractDOMElement::*)(const std::string&)>(&dom::AbstractDOMElement::textContent))
							.method("getInnerHTML", static_cast<std::string(dom::AbstractDOMElement::*)(void)>(&dom::AbstractDOMElement::innerHTML))
                            .method("setInnerHTML", static_cast<void(dom::AbstractDOMElement::*)(const std::string&)>(&dom::AbstractDOMElement::innerHTML))
                            .method("getStyle", static_cast<std::string(dom::AbstractDOMElement::*)(const std::string&)>(&dom::AbstractDOMElement::style))
                            .method("setStyle", static_cast<void(dom::AbstractDOMElement::*)(const std::string&, const std::string&)>(&dom::AbstractDOMElement::style))
							.method("appendChild", &dom::AbstractDOMElement::appendChild)
							.method("removeChild", &dom::AbstractDOMElement::removeChild)
							.method("insertBefore", &dom::AbstractDOMElement::insertBefore)
							.method("cloneNode", &dom::AbstractDOMElement::cloneNode)
							.method("getAttribute", &dom::AbstractDOMElement::getAttribute)
							.method("setAttribute", &dom::AbstractDOMElement::setAttribute)
							.method("getElementsByTagName", &dom::AbstractDOMElement::getElementsByTagName);

						MINKO_LUAGLUE_BIND_SIGNAL(state, dom::AbstractDOMEvent::Ptr);
                        MINKO_LUAGLUE_BIND_SIGNAL(state, dom::AbstractDOMMouseEvent::Ptr);
                        MINKO_LUAGLUE_BIND_SIGNAL(state, dom::AbstractDOMTouchEvent::Ptr);

						abstractDomElement.property("onclick", &dom::AbstractDOMElement::onclick);
						abstractDomElement.property("onmousedown", &dom::AbstractDOMElement::onmousedown);
						abstractDomElement.property("onmousemove", &dom::AbstractDOMElement::onmousemove);
						abstractDomElement.property("onmouseup", &dom::AbstractDOMElement::onmouseup);
						abstractDomElement.property("onmouseout", &dom::AbstractDOMElement::onmouseout);
						abstractDomElement.property("onmouseover", &dom::AbstractDOMElement::onmouseover);
					}

				static
				dom::AbstractDOMElement::Ptr
				atWrapper(std::vector<dom::AbstractDOMElement::Ptr>* v, uint index)
				{
						return v->at(index - 1);
				}
			};
		}
	}
}
