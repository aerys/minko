/*
Copyright (c) 2014 Aerys

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
#include "minko/Signal.hpp"

namespace minko
{
	namespace data
	{
		class AbstractFilter :
			public std::enable_shared_from_this<AbstractFilter>
		{
		public:
			typedef std::shared_ptr<AbstractFilter> Ptr;

		private:
			typedef std::shared_ptr<component::Surface>				SurfacePtr;
			typedef std::shared_ptr<component::AbstractComponent>   AbsCmpPtr;
			typedef std::shared_ptr<scene::Node>					NodePtr;
			typedef std::shared_ptr<Provider>						ProviderPtr;
			typedef std::shared_ptr<Signal<Ptr, SurfacePtr>>		FilterSignalPtr;

		private:
			SurfacePtr												_currentSurface;
			Signal<AbsCmpPtr, NodePtr>::Slot						_currentSurfaceRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot					_currentSurfaceTargetRemovedSlot;

			FilterSignalPtr											_changed;

			std::list<std::string>									_watchedProperties;

		public:
			AbstractFilter():
				_currentSurface(nullptr),
				_currentSurfaceRemovedSlot(nullptr),
				_currentSurfaceTargetRemovedSlot(nullptr),
				_changed(Signal<Ptr, SurfacePtr>::create())
			{
			}

			virtual
            ~AbstractFilter() = default;

			void
			watchProperty(const std::string value);

			void
			unwatchProperty(const std::string value);

			virtual
			bool
			operator()(std::shared_ptr<Provider>) = 0;

			Ptr
			currentSurface(SurfacePtr);

			inline
			SurfacePtr
			currentSurface() const
			{
				return _currentSurface;
			}

			inline
			FilterSignalPtr
			changed() const
			{
				return _changed;
			}

		private:
			virtual
			void
			currentSurfaceRemovedHandler(AbsCmpPtr, NodePtr)
			{
				forgetCurrentSurface();
			}

			virtual
			void
			currentSurfaceTargetRemovedHandler(NodePtr, NodePtr, NodePtr)
			{
				forgetCurrentSurface();
			}

			inline
			void
			forgetCurrentSurface()
			{
				_currentSurface						= nullptr;
				_currentSurfaceRemovedSlot			= nullptr;
				_currentSurfaceTargetRemovedSlot	= nullptr;
			}
		};
	}
}