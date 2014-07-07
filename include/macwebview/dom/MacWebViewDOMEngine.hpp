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

# include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
# import "macwebview/dom/IOSWebView.h"
#elif TARGET_OS_MAC // OSX
# include "macwebview/dom/OSXWebView.h"
#endif

#include "minko/Common.hpp"
#include "minko/dom/AbstractDOM.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"
#include "MacWebViewDOM.hpp"

#import "WebViewJavascriptBridge.h"

namespace macwebview
{
	namespace dom
	{
		class MacWebViewDOMEngine : public minko::dom::AbstractDOMEngine,
                                    public std::enable_shared_from_this<MacWebViewDOMEngine>
		{
		public:
			typedef std::shared_ptr<MacWebViewDOMEngine> Ptr;

		private:
			MacWebViewDOMEngine();

		public:

			~MacWebViewDOMEngine()
			{
			}

			void
			initialize(minko::AbstractCanvas::Ptr, minko::component::SceneManager::Ptr);
			
			void
			enterFrame();

			minko::dom::AbstractDOM::Ptr
			load(std::string uri);

			static
			Ptr
			create();

			void
			clear();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onload();

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onmessage();

			minko::dom::AbstractDOM::Ptr
			mainDOM();

			void
			visible(bool);
            
            bool
            visible();
            
            inline
            WebViewJavascriptBridge*
            bridge()
            {
                return _bridge;
            }
            
            inline
            MacWebViewDOM::Ptr
            currentDOM()
            {
                return _currentDOM;
            }
            
            std::string
			eval(std::string);
            
            inline
            bool
            isReady()
            {
                return _isReady;
            }
            
            void
            handleJavascriptMessage(std::string type, std::string value);
            
            static
            std::function<void(std::string&, std::string&)>
            handleJavascriptMessageWrapper;
            
		private:

			void
			loadScript(std::string filename);

			void
			createNewDom();
            
            void
            registerDomEvents();
            
			static
			int _domUid;

			MacWebViewDOM::Ptr _currentDOM;

			minko::AbstractCanvas::Ptr _canvas;
			minko::component::SceneManager::Ptr _sceneManager;

			minko::Signal<minko::AbstractCanvas::Ptr, minko::uint, minko::uint>::Slot _canvasResizedSlot;
			minko::Signal<minko::component::SceneManager::Ptr, float, float>::Slot _enterFrameSlot;

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onload;
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onmessage;

			bool _visible;
            
            // Mac WebView
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
            UIWindow *_window;
            IOSWebView *_webView;
#elif TARGET_OS_MAC // OSX
            NSWindow *_window;
            OSXWebView *_webView;
#endif
            
            WebViewJavascriptBridge* _bridge;
            
            bool _waitingForLoad;
            std::string _uriToLoad;
            bool _isReady;
		};
	}
}