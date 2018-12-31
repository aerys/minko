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

#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include "include/cef_task.h"

namespace chromium
{
    class ChromiumPimpl;

    class ChromiumClient : public CefClient, public CefContextMenuHandler
    {
    public:
        ChromiumClient(ChromiumPimpl* impl);
        ~ChromiumClient();

        virtual CefRefPtr<CefRenderHandler>
        GetRenderHandler();

        virtual void
        OnBeforeClose(CefRefPtr<CefBrowser> browser);

        bool
        RunContextMenu(
            CefRefPtr<CefBrowser>                browser,
            CefRefPtr<CefFrame>                  frame,
            CefRefPtr<CefContextMenuParams>      params,
            CefRefPtr<CefMenuModel>              model,
            CefRefPtr<CefRunContextMenuCallback> callback) override;

        CefRefPtr<CefContextMenuHandler>
        GetContextMenuHandler() override
        {
            return this;
        }

    private:
        ChromiumPimpl* _impl;

        IMPLEMENT_REFCOUNTING(ChromiumClient);
    };
} // namespace chromium
#endif