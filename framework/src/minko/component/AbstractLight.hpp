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
#include "minko/component/AbstractComponent.hpp"
#include "minko/data/ArrayProvider.hpp"
#include "minko/math/Vector3.hpp"

namespace minko
{
	namespace component
	{
		class AbstractLight :
			public AbstractComponent,
			public std::enable_shared_from_this<AbstractLight>
		{
			friend LightManager;

		public:
			typedef std::shared_ptr<AbstractLight> 					Ptr;

		private:
			typedef	std::shared_ptr<scene::Node>					NodePtr;
			typedef std::shared_ptr<AbstractComponent>				AbsCmpPtr;
			typedef Signal<NodePtr, NodePtr, NodePtr>::Slot 		SceneSignalSlot;
			typedef std::unordered_map<NodePtr, SceneSignalSlot> 	NodeToSceneSignalSlotMap;

		private:
			std::shared_ptr<data::ArrayProvider>	_arrayData;
			std::shared_ptr<math::Vector3>			_color;

			Signal<AbsCmpPtr, NodePtr>::Slot 		_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot 		_targetRemovedSlot;
			NodeToSceneSignalSlotMap 				_addedSlots;
			NodeToSceneSignalSlotMap				_removedSlots;

		public:
			inline
			const uint
			lightId()
			{
				return _arrayData->index();
			}

			inline
			void
			lightId(uint id)
			{
				_arrayData->index(id);
			}

			inline
			std::shared_ptr<math::Vector3>
			color()
			{
				return _color;
			}

		protected:
			AbstractLight(const std::string& arrayName);

			inline
			std::shared_ptr<data::Provider>
			data()
			{
				return _arrayData;
			}

			void
			initialize();

			virtual
			void
			targetAddedHandler(AbsCmpPtr cmp, NodePtr target);

			virtual
			void
			targetRemovedHandler(AbsCmpPtr cmp, NodePtr target);

			virtual
			void
			addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor);
		};
	}
}
