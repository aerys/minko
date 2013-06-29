#pragma once

#include "minko/Common.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/scene/controller/RenderingController.hpp"
#include "minko/Any.hpp"

namespace
{
	using namespace minko::scene;
	using namespace minko::scene::data;
	using namespace minko::math;
}

namespace minko
{
	namespace scene
	{
		namespace controller
		{
			class TransformController :
				public AbstractController,
				public std::enable_shared_from_this<TransformController>
			{

			public:
				typedef std::shared_ptr<TransformController>	Ptr;

			private:
				typedef std::shared_ptr<Node>					NodePtr;
				typedef std::shared_ptr<AbstractController>		AbsCtrlPtr;

			private:
				std::shared_ptr<Matrix4x4>					_transform;
				std::shared_ptr<Matrix4x4>					_modelToWorld;
				std::shared_ptr<Matrix4x4>					_worldToModel;
				std::shared_ptr<DataProvider>				_data;

				Signal<AbsCtrlPtr, NodePtr>::Slot 			_targetAddedSlot;
				Signal<AbsCtrlPtr, NodePtr>::Slot 			_targetRemovedSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot 		_addedSlot;
				Signal<NodePtr, NodePtr, NodePtr>::Slot 		_removedSlot;

			public:
				inline static
				Ptr
				create()
				{
					Ptr ctrl = std::shared_ptr<TransformController>(new TransformController());

					ctrl->initialize();

					return ctrl;
				}

				inline
				std::shared_ptr<Matrix4x4>
				transform()
				{
					return _transform;
				}

				inline
				std::shared_ptr<Matrix4x4>
				modelToWorldMatrix()
				{
					return _modelToWorld;
				}

			private:
				TransformController();

				void
				initialize();

				void
				targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target);

				void
				targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target);

				void
				addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

			private:
				class RootTransformController :
					public std::enable_shared_from_this<RootTransformController>,
					public AbstractController
				{
				public:
					typedef std::shared_ptr<RootTransformController> Ptr;

				private:
					typedef std::shared_ptr<RenderingController>	RenderingCtrlPtr;
					typedef Signal<RenderingCtrlPtr>::Slot 			EnterFrameCallback;

				public:
					inline static
					Ptr
					create()
					{
						auto ctrl = std::shared_ptr<RootTransformController>(new RootTransformController());

						ctrl->initialize();

						return ctrl;
					}

				private:
					std::vector<std::shared_ptr<Matrix4x4>>			_transform;
					std::vector<std::shared_ptr<Matrix4x4>>			_modelToWorld;
					//std::vector<std::shared_ptr<Matrix4x4>>		_worldToModel;

					std::map<NodePtr, unsigned int>					_nodeToId;
					std::vector<NodePtr>							_idToNode;
					std::vector<int>		 						_parentId;
					std::vector<unsigned int> 						_firstChildId;
					std::vector<unsigned int>						_numChildren;
					bool											_invalidLists;

					std::list<Any>									_targetSlots;
					std::map<RenderingCtrlPtr, EnterFrameCallback>	_enterFrameSlots;

				private:
					void
					initialize();

					void
					targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target);

					void
					targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target);

					void
					controllerRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl);

					void
					controllerAddedHandler(NodePtr node, NodePtr target, AbsCtrlPtr	ctrl);

					void
					removedHandler(NodePtr node, NodePtr target, NodePtr parent);

					void
					addedHandler(NodePtr node, NodePtr target, NodePtr parent);

					void
					updateTransformsList();

					void
					updateTransforms();

					void
					enterFrameHandler(RenderingCtrlPtr ctrl);
				};
			};
		}
	}
}
