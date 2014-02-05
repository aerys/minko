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
#include "minko/Signal.hpp"

namespace minko
{
	namespace render
	{
		class ZSortSignalManager: 
			public std::enable_shared_from_this<ZSortSignalManager>
		{
		public:
			typedef std::shared_ptr<ZSortSignalManager>				Ptr;

		private:
			typedef std::shared_ptr<DrawCall>						DrawCallPtr;
			typedef std::shared_ptr<data::Container>				ContainerPtr;
			typedef std::shared_ptr<data::Value>					ValuePtr;

		private:
			typedef std::shared_ptr<Signal<DrawCallPtr>>			ZSortNeedSignalPtr;
			typedef Signal<ContainerPtr, const std::string&>::Slot	PropertyChangedSlot;
			typedef Signal<ValuePtr>::Slot							ValueChangedSlot;

		private:
			static const std::set<std::string>						TARGET_RAW_PNAMES;
			static const std::set<std::string>						RENDERER_RAW_PNAMES;

			const DrawCallPtr										_drawcall;

			PropertyChangedSlot										_targetPropAddedSlot;
			PropertyChangedSlot										_targetPropRemovedSlot;
			PropertyChangedSlot										_rendererPropAddedSlot;
			PropertyChangedSlot										_rendererPropRemovedSlot;
			std::unordered_map<std::string, PropertyChangedSlot>	_propChangedSlots;
			std::unordered_map<std::string, ValueChangedSlot>		_matrixChangedSlots;

			std::set<std::string>									_target_pnames;
			std::set<std::string>									_renderer_pnames;

		public:
			inline static
			Ptr
			create(DrawCallPtr drawCall)
			{
				return std::shared_ptr<ZSortSignalManager>(new ZSortSignalManager(drawCall));
			}

			void
			initialize(ContainerPtr targetData, ContainerPtr rendererData, ContainerPtr rootData);

			void
			clear();

		private:
			ZSortSignalManager(DrawCallPtr drawcall);

			void
			propertyAddedHandler(ContainerPtr, const std::string&, const std::set<std::string>&);

			void
			propertyRemovedHandler(ContainerPtr, const std::string&, const std::set<std::string>&);

			void
			requestZSort();
		};
	}
}