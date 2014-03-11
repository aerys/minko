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

#if defined(EMSCRIPTEN)
#pragma once

#include "minko/Common.hpp"
#include "emscripten/dom/EmscriptenDOMElement.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"

using namespace minko;
using namespace minko::dom;
using namespace emscripten;
using namespace emscripten::dom;

EmscriptenDOMElement::EmscriptenDOMElement()
{
}

EmscriptenDOMElement::Ptr
create(std::string accessor, std::string varName)
{
	return nullptr;
}

std::string
EmscriptenDOMElement::id()
{
	return "";
}

void
EmscriptenDOMElement::id(std::string)
{
}

std::string
EmscriptenDOMElement::className()
{
	return "";
}

void
EmscriptenDOMElement::className(std::string)
{
}

std::string
EmscriptenDOMElement::tagName()
{
	return "";
}

minko::dom::AbstractDOMElement::Ptr
EmscriptenDOMElement::parentNode()
{
	return nullptr;
}

std::list<minko::dom::AbstractDOMElement::Ptr>
EmscriptenDOMElement::childNodes()
{
	std::list<minko::dom::AbstractDOMElement::Ptr> l;
	return l;
}

std::string
EmscriptenDOMElement::textContent()
{
	return "";
}

void
EmscriptenDOMElement::textContent(std::string)
{
	return;
}

std::string
EmscriptenDOMElement::innerHTML()
{
	return "";
}

void
EmscriptenDOMElement::innerHTML(std::string)
{
	return;
}

minko::dom::AbstractDOMElement::Ptr
EmscriptenDOMElement::appendChild(minko::dom::AbstractDOMElement::Ptr)
{
	return nullptr;
}

minko::dom::AbstractDOMElement::Ptr
EmscriptenDOMElement::removeChild(minko::dom::AbstractDOMElement::Ptr)
{
	return nullptr;
}

minko::dom::AbstractDOMElement::Ptr
EmscriptenDOMElement::insertBefore(minko::dom::AbstractDOMElement::Ptr, minko::dom::AbstractDOMElement::Ptr)
{
	return nullptr;
}

minko::dom::AbstractDOMElement::Ptr
EmscriptenDOMElement::cloneNode(bool deep)
{
	return nullptr;
}

std::string
EmscriptenDOMElement::getAttribute(std::string name)
{
	return "";
}

void
EmscriptenDOMElement::setAttribute(std::string name, std::string value)
{
	return;
}

std::list<minko::dom::AbstractDOMElement::Ptr>
EmscriptenDOMElement::getElementsByTagName(std::string tagName)
{
	std::list<minko::dom::AbstractDOMElement::Ptr> l;
	return l;
}


std::string
EmscriptenDOMElement::style(std::string name)
{
	return "";
}

void
EmscriptenDOMElement::style(std::string name, std::string value)
{
	return;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
EmscriptenDOMElement::onclick()
{
	return nullptr;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
EmscriptenDOMElement::onmousedown()
{
	return nullptr;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
EmscriptenDOMElement::onmousemove()
{
	return nullptr;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
EmscriptenDOMElement::onmouseup()
{
	return nullptr;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
EmscriptenDOMElement::onmouseout()
{
	return nullptr;
}

Signal<std::shared_ptr<AbstractDOMEvent>>::Ptr
EmscriptenDOMElement::onmouseover()
{
	return nullptr;
}
#endif