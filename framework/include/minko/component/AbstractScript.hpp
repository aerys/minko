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

#include "minko/component/AbstractComponent.hpp"
#include "minko/Signal.hpp"

namespace minko
{
	namespace component
	{
		class AbstractScript :
			public AbstractComponent
		{
		public:
			typedef std::shared_ptr<AbstractScript>                         Ptr;

		private:
			typedef std::shared_ptr<scene::Node>		                    NodePtr;
			typedef std::shared_ptr<AbstractComponent>	                    AbsCmpPtr;

		private:
			bool															_enabled;
			bool				                							_started;
            float                                                           _time;
            float                                                           _deltaTime;

			Signal<AbsCmpPtr, NodePtr>::Slot				                _targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot				                _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot                         _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot                         _removedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot		                _componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot		                _componentRemovedSlot;
			Signal<std::shared_ptr<SceneManager>, float, float>::Slot		_frameBeginSlot;
			Signal<std::shared_ptr<SceneManager>, float, float>::Slot		_frameEndSlot;

		public:
			bool
			enabled()
			{
				return _enabled;
			}

			void
			enabled(bool v);

		protected:
			AbstractScript() :
				_enabled(true),
				_started(false),
				_time(0.f),
				_deltaTime(0.f),
				_targetAddedSlot(nullptr),
				_targetRemovedSlot(nullptr),
				_addedSlot(nullptr),
				_removedSlot(nullptr),
				_componentAddedSlot(nullptr),
				_componentRemovedSlot(nullptr),
				_frameBeginSlot(nullptr),
				_frameEndSlot(nullptr)
			{
			}

            inline
            float
            time()
            {
                return _time;
            }

            inline
            float
            deltaTime()
            {
                return _deltaTime;
            }

			virtual
			void
			start(NodePtr target)
			{
				// nothing
			}

			virtual
			void
			update(NodePtr target)
			{
				// nothing
			}

			virtual
			void
			end(NodePtr target)
			{
				// nothing
			}

			virtual
			void
			stop(NodePtr target)
			{
				// nothing
			}

			virtual
			bool
			ready()
			{
				return true;
			}

            virtual
            float
            priority()
            {
                return 0.0f;
            }

		protected:
			virtual
			void
			targetAdded(NodePtr node);

			virtual
			void
			targetRemoved(NodePtr node);

            void
            addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			componentAddedHandler(NodePtr node, NodePtr	target, AbsCmpPtr component);

			void
			componentRemovedHandler(NodePtr	node, NodePtr target, AbsCmpPtr	component);

			void
			frameBeginHandler(std::shared_ptr<SceneManager> sceneManager, float, float);

			void
			frameEndHandler(std::shared_ptr<SceneManager> sceneManager, float, float);

			void
			setSceneManager(std::shared_ptr<SceneManager> sceneManager);
		};
	}
}
