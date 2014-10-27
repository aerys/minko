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
#include "minko/dom/AbstractDOM.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"
#include "AndroidWebViewDOM.hpp"
#include <jni.h>

namespace android
{
	namespace dom
	{
		class AndroidWebViewDOMEngine : public minko::dom::AbstractDOMEngine,
                                        public std::enable_shared_from_this<AndroidWebViewDOMEngine>
		{
		public:
			typedef std::shared_ptr<AndroidWebViewDOMEngine> Ptr;

		private:
			AndroidWebViewDOMEngine();

		public:

			~AndroidWebViewDOMEngine()
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
            AndroidWebViewDOM::Ptr
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

		private:

			void
			createNewDom();

            void
            registerDomEvents();

            void
            updateWebViewResolution(int width, int height);

            void
            updateEvents();

        public:
			static
			int _domUid;

            static 
            std::vector<std::string> messages;

            static 
            std::vector<minko::dom::AbstractDOMEvent::Ptr> events;

            // WebView Signals
            static minko::Signal<>::Ptr onWebViewInitialized;
            static minko::Signal<>::Ptr onWebViewPageLoaded;

            static
            std::mutex eventMutex;

            static
            std::mutex messageMutex;

            static
            Ptr currentEngine;

            static
            int numTouches;

            static
            int firstIdentifier;
        private:

            // WebView Slots
            minko::Signal<>::Slot _onWebViewInitializedSlot;
            minko::Signal<>::Slot _onWebViewPageLoadedSlot;

            // Inputs Slots
            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot _onmousemoveSlot;
            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot _onmousedownSlot;
            minko::Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot _onmouseupSlot;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot _ontouchstartSlot;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot _ontouchendSlot;
            minko::Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot _ontouchmoveSlot;

			AndroidWebViewDOM::Ptr _currentDOM;

			minko::AbstractCanvas::Ptr _canvas;
			minko::component::SceneManager::Ptr _sceneManager;

			minko::Signal<minko::AbstractCanvas::Ptr, minko::uint, minko::uint>::Slot _canvasResizedSlot;
			minko::Signal<minko::component::SceneManager::Ptr, float, float>::Slot _enterFrameSlot;

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onload;
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onmessage;

            bool _visible;

            bool _waitingForLoad;
            std::string _uriToLoad;
            bool _isReady;

            bool _webViewInitialized;
            bool _webViewPageLoaded;

            float _lastUpdateTime;
            int _pollRate;
            bool _updateNextFrame;

            // JNI part

            // Java Objects
            jobject _initWebViewTask = nullptr;

            // Java Method IDs
            jmethodID _evalJSMethod = nullptr;
            jmethodID _changeResolutionMethod = nullptr;
            jmethodID _loadUrlMethod = nullptr;
		};
	}
}