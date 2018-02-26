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

#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
# import "apple/dom/IOSWebView.h"
#elif TARGET_OS_MAC // OSX
# include "apple/dom/OSXWebView.h"
#endif

#include "minko/Common.hpp"
#include "minko/input/Touch.hpp"
#include "minko/dom/AbstractDOM.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"
#include "minko/render/Texture.hpp"

#include "apple/dom/AppleWebViewDOM.hpp"

#import "WebViewJavascriptBridge.h"

namespace apple
{
	namespace dom
	{
		class AppleWebViewDOMEngine :
            public minko::dom::AbstractDOMEngine,
            public std::enable_shared_from_this<AppleWebViewDOMEngine>
		{
		public:
			typedef std::shared_ptr<AppleWebViewDOMEngine> Ptr;

		private:
			AppleWebViewDOMEngine();

		public:

			~AppleWebViewDOMEngine()
			{
			}

			void
			initialize(minko::AbstractCanvas::Ptr, minko::component::SceneManager::Ptr);
			
			void
			enterFrame(float);

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
            AppleWebViewDOM::Ptr
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

            inline
            void
            updateNextFrame()
            {
                _updateNextFrame = true;
            }
            
            inline
            void
            pollRate(int rate)
            {
            	    _pollRate = rate;
            }
            
            // Render to texture
            
            void
            enableRenderToTexture(std::shared_ptr<minko::render::AbstractTexture> texture) override;
            
            void
            disableRenderToTexture() override;
            
		private:

			void
			loadScript(std::string filename);

			void
			createNewDom();
            
            void
            registerDomEvents();
            
        public:
            inline
            int
            firstTouchIdentifier()
            {
                if (numTouches())
                    return _canvas->touch()->identifiers()[0];

                return -1;
            }
            
            inline
            int
            numTouches()
            {
                return _canvas->touch()->numTouches();
            }

        private:
			static
			int _domUid;

			AppleWebViewDOM::Ptr _currentDOM;

			minko::AbstractCanvas::Ptr _canvas;
			minko::component::SceneManager::Ptr _sceneManager;

			minko::Signal<minko::component::SceneManager::Ptr, float, float>::Slot _enterFrameSlot;

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onload;
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onmessage;

            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot _onmousemoveSlot;
            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot _onmousedownSlot;
            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot _onmouseupSlot;

            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot _ontouchstartSlot;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot _ontouchendSlot;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot _ontouchmoveSlot;

			bool _visible;
            
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
            // WebView
            UIWindow* _window;
            IOSWebView* _webView;
            minko::render::Texture::Ptr _webViewTexture;
            GLubyte* _webViewTexturePixelBuffer;
            CGContextRef _graphicContext;
#elif TARGET_OS_MAC // OSX
            // WebView
            NSWindow* _window;
            OSXWebView* _webView;
#endif
            
            CGRect _originalWebViewSize;
            float _webViewRenderToTextureTime;
            
            WebViewJavascriptBridge* _bridge;
            
            bool _waitingForLoad;
            std::string _uriToLoad;
            bool _isReady;

            float _lastUpdateTime;
            int _pollRate;
            bool _updateNextFrame;
		};
	}
}
