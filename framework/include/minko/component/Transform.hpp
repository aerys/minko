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
#include "minko/Any.hpp"
#include "minko/math/Matrix4x4.hpp"

namespace minko
{
	namespace component
	{
		class Transform :
			public AbstractComponent
		{

		public:
			typedef std::shared_ptr<Transform>	Ptr;

		private:
			typedef std::shared_ptr<scene::Node>			NodePtr;
			typedef std::shared_ptr<AbstractComponent>		AbsCtrlPtr;

		private:
			std::shared_ptr<math::Matrix4x4>				_matrix;
			std::shared_ptr<math::Matrix4x4>				_modelToWorld;
			std::shared_ptr<math::Matrix4x4>				_worldToModel;
			std::shared_ptr<data::StructureProvider>		_data;

			Signal<AbsCtrlPtr, NodePtr>::Slot 				_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot 				_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 		_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 		_removedSlot;

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
			create(std::shared_ptr<math::Matrix4x4> transform)
			{
				auto ctrl = create();

				ctrl->_matrix->copyFrom(transform);

				return ctrl;
			}

			~Transform() = default;
			
			AbstractComponent::Ptr
			clone();

			inline
			std::shared_ptr<math::Matrix4x4>
			matrix()
			{
				return _matrix;
			}

			inline
			std::shared_ptr<math::Vector3>
			modelToWorld(std::shared_ptr<math::Vector3> v, std::shared_ptr<math::Vector3> out = nullptr)
			{
				return _modelToWorld->transform(v, out);
			}

			inline
			std::shared_ptr<math::Vector3>
			deltaModelToWorld(std::shared_ptr<math::Vector3> v, std::shared_ptr<math::Vector3> out = nullptr)
			{
				return _modelToWorld->deltaTransform(v, out);
			}

			inline
			std::shared_ptr<math::Vector3>
			worldToModel(std::shared_ptr<math::Vector3> v, std::shared_ptr<math::Vector3> out = nullptr)
			{
				return _worldToModel->copyFrom(_modelToWorld)->invert()->transform(v, out);
			}

			inline
			std::shared_ptr<math::Vector3>
			deltaWorldToModel(std::shared_ptr<math::Vector3> v, std::shared_ptr<math::Vector3> out = nullptr)
			{
				return _worldToModel->copyFrom(_modelToWorld)->invert()->deltaTransform(v, out);
			}

			inline
			std::shared_ptr<math::Matrix4x4>
			modelToWorldMatrix()
			{
				return modelToWorldMatrix(false);
			}

			inline
			std::shared_ptr<math::Matrix4x4>
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

            inline
            float
            x()
            {
                return this->modelToWorld(minko::math::Vector3::create())->x();
            }

            inline
            float
            y()
            {
                return this->modelToWorld(minko::math::Vector3::create())->y();
            }

            inline
            float
            z()
            {
                return this->modelToWorld(minko::math::Vector3::create())->z();
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
				public AbstractComponent
			{
			public:
				typedef std::shared_ptr<RootTransform> Ptr;

			private:
				typedef std::shared_ptr<Renderer>		RendererCtrlPtr;
				typedef Signal<RendererCtrlPtr>::Slot 	EnterFrameCallback;

			public:
				inline static
				Ptr
				create()
				{
					auto ctrl = std::shared_ptr<RootTransform>(new RootTransform());

					ctrl->initialize();

					return ctrl;
				}

				AbstractComponent::Ptr
				clone();

				void
				forceUpdate(NodePtr node, bool updateTransformLists = false);

			private:
				std::vector<std::shared_ptr<math::Matrix4x4>>	_transforms;
				std::vector<std::shared_ptr<math::Matrix4x4>>	_modelToWorld;

				std::map<NodePtr, unsigned int>					_nodeToId;
				std::vector<NodePtr>							_idToNode;
				std::vector<int>		 						_parentId;
				std::vector<unsigned int> 						_firstChildId;
				std::vector<unsigned int>						_numChildren;
				bool											_invalidLists;

				std::list<Any>									_targetSlots;
				Signal<std::shared_ptr<SceneManager>, uint, std::shared_ptr<render::AbstractTexture>>::Slot		_renderingBeginSlot;

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
				renderingBeginHandler(std::shared_ptr<SceneManager> sceneManager, 
									  uint							frameId, 
									  std::shared_ptr<render::AbstractTexture>);

				static
				void
				juxtaposeSiblings(std::vector<NodePtr>&);
			};
		};
	}
}
