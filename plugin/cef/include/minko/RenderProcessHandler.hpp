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
#include "include/cef_render_process_handler.h"
#include "minko/Signal.hpp"

namespace minko
{
	class CefPimpl;

	class V8Handler : public CefV8Handler
	{
	public:
		V8Handler() {}

		static
			Signal<std::string>::Ptr
			messageReceived()
		{
				return _messageReceived;
			}

		virtual bool Execute(const CefString& name,
			CefRefPtr<CefV8Value> object,
			const CefV8ValueList& arguments,
			CefRefPtr<CefV8Value>& retval,
			CefString& exception) OVERRIDE
		{
			if (name == "sendMessage")
			{
				CefRefPtr<CefV8Value> message = arguments[0];

				std::string value = message->GetStringValue();

				_messageReceived->execute(value);

				std::cout << value << "\n";
			}
			else if (name == "sendEventCallback")
			{
				CefRefPtr<CefV8Value> callbackId = arguments[0];
				CefRefPtr<CefV8Value> elementId = arguments[1];
				CefRefPtr<CefV8Value> eventType = arguments[2];

				std::string callbackIdValue = callbackId->GetStringValue();
				std::string elementIdValue = elementId->GetStringValue();
				std::string eventTypeValue = eventType->GetStringValue();


			}

			// Function does not exist.
			return false;
		}

	private:

		static
			Signal<std::string>::Ptr _messageReceived;

		IMPLEMENT_REFCOUNTING(V8Handler);
	};

	class DOMListener : public CefDOMEventListener
	{
	public:
		DOMListener(std::string id) :
			_id(id)
		{
		}

		std::shared_ptr<Signal<std::string, std::string>>
		received()
		{
			return _received;
		}

		void
		HandleEvent(CefRefPtr<CefDOMEvent> e)
		{
			CefRefPtr<CefDOMNode> node = e->GetTarget();
			std::string id = "";
			while (id != "target" && node != nullptr)
			{
				if (node->IsElement() && node->HasElementAttribute("id"))
				{
					id = node->GetElementAttribute("id");
				}
				else
				{
					id = "";
				}
				
				node = node->GetParent();
			}

			if (id == _id)
			{
				std::string type = e->GetType();
				_received->execute(type, id);
			}
		}

	private:
		std::string _id;
		Signal<std::string, std::string>::Ptr _received;

		IMPLEMENT_REFCOUNTING(DOMListener);
	};

	class DOMVisitor : public CefDOMVisitor
	{
	public:
		DOMVisitor(std::string type, std::string id) :
			_type(type)
		{
			_listener = CefRefPtr<DOMListener>(new DOMListener(id));
		}

		std::shared_ptr<Signal<std::string, std::string>>
		received()
		{
			return _listener.get()->received();
		}

		void Visit(CefRefPtr<CefDOMDocument> document)
		{
			document->GetDocument()->AddEventListener(_type, (CefRefPtr<CefDOMEventListener>)_listener, true);
		}

	private:
		std::string _type;
		CefRefPtr<DOMListener> _listener;

		IMPLEMENT_REFCOUNTING(DOMVisitor);
	};

	class LoadHandler : public CefLoadHandler
	{
	public:

		virtual
		void
		OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
		{
			_frame = frame;
			for (CefRefPtr<CefDOMVisitor> visitor : _visitors)
			{
				frame->VisitDOM(visitor);
			}
			_loadStarted = true;
		}

		Signal<std::string, std::string>::Ptr
		addVisitor(std::string type, std::string id)
		{
			CefRefPtr<CefDOMVisitor> visitor = CefRefPtr<CefDOMVisitor>(new DOMVisitor(type, id));

			_visitors.push_back(visitor);

			if (_loadStarted)
			{
				_frame->VisitDOM(visitor);
			}
		}

	private:
		CefRefPtr<CefFrame> _frame;
		bool _loadStarted;
		std::list<CefRefPtr<CefDOMVisitor>> _visitors;

		IMPLEMENT_REFCOUNTING(LoadHandler);
	};

	class RenderProcessHandler : public CefRenderProcessHandler
	{
	public:
		RenderProcessHandler(CefPimpl* impl);
	public:
		virtual
		void
		OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

		virtual
		CefRefPtr<CefLoadHandler>
		GetLoadHandler();

		Signal<std::string, std::string>::Ptr
		addEventListener(std::string type, std::string id)
		{
			return _loadHandler.get()->addVisitor(type, id);
		}

	private:
		CefRefPtr<LoadHandler> _loadHandler;
		CefPimpl*  _impl;

		IMPLEMENT_REFCOUNTING(RenderProcessHandler);
	};
}