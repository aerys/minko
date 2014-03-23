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

#include "minko/scene/Node.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/Any.hpp"

namespace minko
{
	namespace component
	{
		class Transform :
			public AbstractComponent,
			public std::enable_shared_from_this<Transform>
		{

		public:
			typedef std::shared_ptr<Transform>			Ptr;

		private:
			typedef std::shared_ptr<scene::Node>		NodePtr;
			typedef std::shared_ptr<AbstractComponent>	AbsCtrlPtr;

		private:
			math::mat4								_matrix;
			math::mat4								_modelToWorld;
			std::shared_ptr<data::StructureProvider>	_data;

			Signal<AbsCtrlPtr, NodePtr>::Slot 			_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot 			_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 	_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 	_removedSlot;

		public:
			inline static
			Ptr
			create()
			{
				Ptr ctrl = std::shared_ptr<Transform>(new Transform());

				ctrl->initialize();

				return ctrl;
			}

			inline static
			Ptr
			create(const math::mat4& transform)
			{
				auto ctrl = create();

				ctrl->_matrix = transform;

				return ctrl;
			}

			~Transform()
			{
			}

			inline
			const math::mat4&
			matrix()
			{
				return _matrix;
			}

			inline
			void
			matrix(const math::mat4& matrix)
			{
				_matrix = matrix;
				_data->set("matrix", _matrix);

				auto rootTransform = targets()[0]->root()->component<RootTransform>();

				if (rootTransform && !rootTransform->_invalidLists)
					rootTransform->_dirty[rootTransform->_nodeToId[targets()[0]]] = true;
				else
					_modelToWorld = matrix;
			}

			inline
			const math::mat4&
			modelToWorldMatrix()
			{
				return modelToWorldMatrix(false);
			}

			inline
			const math::mat4&
			modelToWorldMatrix(bool forceUpdate)
			{
				if (forceUpdate)
				{
					auto node		= targets()[0];
					auto rootCtrl	= node->root()->component<RootTransform>();

					rootCtrl->forceUpdate(node, true);
				}

				return _modelToWorld;
			}

		private:
			Transform();

			void
			initialize();

			void
			targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target);

			void
			addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

#ifdef MINKO_TEST
		public:
#else
		private:
#endif // MINKO_TEST
			class RootTransform :
				public std::enable_shared_from_this<RootTransform>,
				public AbstractComponent
			{
				friend class Transform;

			public:
				typedef std::shared_ptr<RootTransform> Ptr;

			private:
				typedef std::shared_ptr<SceneManager>				SceneMgrPtr;
				typedef std::shared_ptr<Renderer>					RendererCtrlPtr;
				typedef Signal<RendererCtrlPtr>::Slot 				EnterFrameCallback;
				typedef std::shared_ptr<render::AbstractTexture> 	AbsTexturePtr;
				typedef Signal<SceneMgrPtr, uint, AbsTexturePtr> 	RenderingBeginSignal;
				typedef RenderingBeginSignal::Slot 					RenderingBeginSlot;
				typedef std::shared_ptr<Transform>					TransformPtr;

			public:
				inline static
				Ptr
				create()
				{
					auto ctrl = std::shared_ptr<RootTransform>(new RootTransform());

					ctrl->initialize();

					return ctrl;
				}

				void
				forceUpdate(NodePtr node, bool updateTransformLists = false);

			private:
				std::vector<TransformPtr>		_transforms;
				std::vector<math::mat4*>	_modelToWorld;

				std::map<NodePtr, unsigned int>	_nodeToId;
				std::vector<NodePtr>			_idToNode;
				std::vector<int>		 		_parentId;
				std::vector<unsigned int> 		_firstChildId;
				std::vector<unsigned int>		_numChildren;
				std::vector<bool>				_dirty;
				bool							_invalidLists;

				std::list<Any>					_targetSlots;
				RenderingBeginSlot				_renderingBeginSlot;

			private:
				void
				initialize();

				void
				targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target);

				void
				targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target);

				void
				componentRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl);

				void
				componentAddedHandler(NodePtr node, NodePtr target, AbsCtrlPtr	ctrl);

				void
				removedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				addedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				updateTransformsList();

				void
				updateTransforms();

				void
				updateTransformPath(const std::vector<unsigned int>& path);

				void
				renderingBeginHandler(SceneMgrPtr 	sceneManager, 
									  uint			frameId, 
									  AbsTexturePtr	target);
			};
		};
	}
}
