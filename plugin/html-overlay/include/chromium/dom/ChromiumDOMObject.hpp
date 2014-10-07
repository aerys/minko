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
#pragma once

#include "minko/Common.hpp"
#include "include/cef_render_process_handler.h"
#include "include/cef_runnable.h"
#include "include/cef_task.h"

namespace minko
{
    namespace dom
    {
        class AbstractDOMElement;
    }
}

namespace chromium
{
    namespace dom
    {
        class ChromiumDOMObject
        {
        protected:

            CefRefPtr<CefV8Value>
            getFunction(const std::string& name);

            template <typename T>
            T
            getProperty(const std::string& name)
            {

                T result;

                if (CefCurrentlyOn(TID_RENDERER))
                {
                    _v8Context->Enter();
                    result = getV8Property<T>(name);
                    _v8Context->Exit();
                }
                else
                {
                    CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
                    _blocker.store(true);

                    auto fn = [&]()
                    {
                        result = getProperty<T>(name);
                        _blocker.store(false);
                    };

                    runner->PostTask(NewCefRunnableFunction(&fn));

                    while (_blocker.load());
                }

                return result;
            }

            template <typename T>
            void
            setProperty(const std::string& name, T value)
            {

                if (CefCurrentlyOn(TID_RENDERER))
                {
                    _v8Context->Enter();
                    setV8Property<T>(name, value);
                    _v8Context->Exit();
                }
                else
                {
                    CefRefPtr<CefTaskRunner> runner = CefTaskRunner::GetForThread(TID_RENDERER);
                    //_blocker.store(true);

                    auto fn = [=]()
                    {
                        setProperty<T>(name, value);
                        //_blocker.store(false);
                    };

                    runner->PostTask(NewCefRunnableFunction(&fn));
                    //while (_blocker.load());
                }
            }

            template <typename T>
            T
            getV8Property(const std::string& name);

            template <typename T>
            void
            setV8Property(const std::string& name, T);

            std::atomic<bool> _blocker;

            CefRefPtr<CefV8Value> _v8NodeObject;
            CefRefPtr<CefV8Context> _v8Context;
        };

        template<>
        CefRefPtr<CefV8Value>
        ChromiumDOMObject::getV8Property(const std::string& name);

        template<>
        std::string
        ChromiumDOMObject::getV8Property(const std::string& name);

        template<>
        int
        ChromiumDOMObject::getV8Property(const std::string& name);

        template<>
        bool
        ChromiumDOMObject::getV8Property(const std::string& name);

        template<>
        std::vector<std::shared_ptr<minko::dom::AbstractDOMElement>>
        ChromiumDOMObject::getV8Property(const std::string& name);

        template<>
        std::shared_ptr<minko::dom::AbstractDOMElement>
        ChromiumDOMObject::getV8Property(const std::string& name);

        template<>
        void
        ChromiumDOMObject::setV8Property<CefRefPtr<CefV8Value>>(const std::string& name, CefRefPtr<CefV8Value> value);

        template<>
        void
        ChromiumDOMObject::setV8Property<const std::string&>(const std::string& name, const std::string& value);

        template<>
        void
        ChromiumDOMObject::setV8Property<bool>(const std::string& name, bool value);

        template<>
        void
        ChromiumDOMObject::setV8Property<int>(const std::string& name, int value);
    }
}

#endif