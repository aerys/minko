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

#if defined(CHROMIUM)

#include "chromium/dom/ChromiumDOMObject.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

template<>
CefRefPtr<CefV8Value>
ChromiumDOMObject::getV8Property(const std::string& name)
{
    return _v8NodeObject->GetValue(name);
}

template<>
std::string
ChromiumDOMObject::getV8Property(const std::string& name)
{
    return getV8Property<CefRefPtr<CefV8Value>>(name)->GetStringValue();
}

template<>
bool
ChromiumDOMObject::getV8Property(const std::string& name)
{
    return getV8Property<CefRefPtr<CefV8Value>>(name)->GetBoolValue();
}

template<>
int
ChromiumDOMObject::getV8Property(const std::string& name)
{
    return getV8Property<CefRefPtr<CefV8Value>>(name)->GetIntValue();
}

template<>
std::vector<AbstractDOMElement::Ptr>
ChromiumDOMObject::getV8Property(const std::string& name)
{
    return ChromiumDOMElement::v8ElementArrayToList(getV8Property<CefRefPtr<CefV8Value>>(name), _v8Context);
}

template<>
AbstractDOMElement::Ptr
ChromiumDOMObject::getV8Property(const std::string& name)
{
    return ChromiumDOMElement::getDOMElementFromV8Object(getV8Property<CefRefPtr<CefV8Value>>(name), _v8Context);
}

template<>
void
ChromiumDOMObject::setV8Property<CefRefPtr<CefV8Value>>(const std::string& name, CefRefPtr<CefV8Value> value)
{
    _v8NodeObject->SetValue(name, value, V8_PROPERTY_ATTRIBUTE_NONE);
}

template<>
void
ChromiumDOMObject::setV8Property<const std::string&>(const std::string& name, const std::string& value)
{
    setV8Property(name, CefV8Value::CreateString(value));
}

template<>
void
ChromiumDOMObject::setV8Property<bool>(const std::string& name, bool value)
{
    setV8Property(name, CefV8Value::CreateBool(value));
}

template<>
void
ChromiumDOMObject::setV8Property<int>(const std::string& name, int value)
{
    setV8Property(name, CefV8Value::CreateInt(value));
}

CefRefPtr<CefV8Value>
ChromiumDOMObject::getFunction(const std::string& name)
{
    CefRefPtr<CefV8Value> func = getV8Property<CefRefPtr<CefV8Value>>(name);

    if (!func->IsFunction())
        throw;

    return func;
}

#endif