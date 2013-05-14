#pragma once

#include "minko/Common.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/scene/controller/RenderingController.hpp"

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
				typedef std::shared_ptr<TransformController>	ptr;

			private:
				typedef std::shared_ptr<Node>					NodePtr;
				typedef std::shared_ptr<AbstractController>		AbstractControllerPtr;

			public:
				inline static
				ptr
				create()
				{
					ptr ctrl = std::shared_ptr<TransformController>(new TransformController());

					ctrl->initialize();

					return ctrl;
				}

				inline
				std::shared_ptr<Matrix4x4>
				transform()
				{
					return _transform;
				}

			private:
				std::shared_ptr<Matrix4x4>								_transform;
				std::shared_ptr<Matrix4x4>								_modelToWorld;
				std::shared_ptr<Matrix4x4>								_worldToModel;
				std::shared_ptr<DataProvider>							_data;

				Signal<AbstractControllerPtr, NodePtr>::cd 				_targetAddedCd;
				Signal<AbstractControllerPtr, NodePtr>::cd 				_targetRemovedCd;
				Signal<NodePtr, NodePtr>::cd 							_addedCd;
				Signal<NodePtr, NodePtr>::cd 							_removedCd;

			private:
				TransformController();

				void
				initialize();

				void
				targetAddedHandler(std::shared_ptr<AbstractController> ctrl, NodePtr target);

				void
				targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, NodePtr target);

				void
				addedOrRemovedHandler(NodePtr node, NodePtr ancestor);

			private:
				class RootTransformController :
					public std::enable_shared_from_this<RootTransformController>,
					public AbstractController
				{
				public:
					typedef std::shared_ptr<RootTransformController> ptr;

				private:
					typedef std::shared_ptr<Node>					NodePtr;
					typedef std::shared_ptr<AbstractController>		AbstractControllerPtr;
					typedef std::shared_ptr<RenderingController>	RenderingControllerPtr;
					typedef Signal<RenderingControllerPtr>::cd 		EnterFrameCallback;

				public:
					inline static
					ptr
					create()
					{
						auto ctrl = std::shared_ptr<RootTransformController>(new RootTransformController());

						ctrl->initialize();

						return ctrl;
					}

				private:
					std::vector<std::shared_ptr<Matrix4x4>>					_transform;
					std::vector<std::shared_ptr<Matrix4x4>>					_modelToWorld;
					//std::vector<std::shared_ptr<Matrix4x4>>					_worldToModel;

					std::map<std::shared_ptr<Node>, unsigned int>			_nodeToId;
					std::vector<std::shared_ptr<Node>>						_idToNode;
					std::vector<unsigned int> 								_parentId;
					std::vector<unsigned int> 								_firstChildId;
					std::vector<unsigned int>								_numChildren;
					bool													_invalidLists;

					std::list<Any>											_targetCds;
					std::map<RenderingControllerPtr, EnterFrameCallback>	_enterFrameCds;

				private:
					void
					initialize();

					void
					targetAddedHandler(AbstractControllerPtr ctrl, NodePtr target);

					void
					targetRemovedHandler(AbstractControllerPtr ctrl, NodePtr target);

					void
					controllerRemovedHandler(NodePtr node, AbstractControllerPtr	ctrl);

					void
					controllerAddedHandler(NodePtr node, AbstractControllerPtr	ctrl);

					void
					descendantRemovedHandler(NodePtr node, NodePtr descendant);

					void
					descendantAddedHandler(NodePtr node, NodePtr descendant);

					void
					updateTransformsList();

					void
					updateTransforms();

					void
					enterFrameHandler(std::shared_ptr<RenderingController> ctrl);
				};
			};
		}
	}
}
