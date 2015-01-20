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

#include "minko/data/AbstractFilter.hpp"

namespace minko
{
	namespace data
	{
		class LightMaskFilter:
			public AbstractFilter
		{
		public:
			typedef std::shared_ptr<LightMaskFilter>			Ptr;

		private:
			typedef std::shared_ptr<scene::Node>				NodePtr;
			typedef std::shared_ptr<Provider>					ProviderPtr;
			typedef std::shared_ptr<component::AbstractLight>	AbsLightPtr;
            typedef const std::string&                          StringRef;

            typedef Signal<Store&, ProviderPtr, StringRef>	StorePropertyChangedSignal;
            typedef Signal<ProviderPtr, StringRef>              ProviderPropertyChangedSignal;

		private:
			static std::vector<std::string>						_numLightPropertyNames;

			NodePtr												_target;
			NodePtr												_root;
			std::unordered_map<ProviderPtr, AbsLightPtr>		_providerToLight;

			std::list<StorePropertyChangedSignal::Slot>		_rootPropertyChangedSlots;

			std::list<ProviderPropertyChangedSignal::Slot>		_layoutMaskChangedSlots;

		public:
			inline static
			Ptr
			create(NodePtr root = nullptr)
			{
				Ptr ptr = std::shared_ptr<LightMaskFilter>(new LightMaskFilter());

				ptr->watchProperty("node.layouts");
				ptr->root(root);

				return ptr;
			}

			Ptr
			root(NodePtr);

			bool
			operator()(ProviderPtr); 

		private:
			LightMaskFilter();

			void
			reset();

			void
			initialize(NodePtr);

			void
			lightsChangedHandler();

			static
			std::vector<std::string>
			initializeNumLightPropertyNames();
		};
	}
}