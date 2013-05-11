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
				typedef std::shared_ptr<RenderingController>	RenderingControllerPtr;
				typedef Signal<RenderingControllerPtr>			EnterFrameCd;

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

				NodePtr													_referenceFrame;

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

				void
				controllerAddedOrRemovedHandler(std::shared_ptr<Node> 				node,
									   			std::shared_ptr<AbstractController>  ctrl);

				void
				updateReferenceFrame(NodePtr node);

			private:
				class RootTransformController :
					public std::enable_shared_from_this<RootTransformController>,
					public AbstractController
				{
				public:
					typedef std::shared_ptr<RootTransformController> ptr;

				private:
					typedef std::shared_ptr<Node>				NodePtr;
					typedef std::shared_ptr<AbstractController>	AbstractControllerPtr;

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
					Signal<NodePtr, NodePtr>::cd 							_descendantAddedCd;
					Signal<NodePtr, NodePtr>::cd 							_descendantRemovedCd;
					std::list<Signal<NodePtr, AbstractControllerPtr>::cd> 	_controllerAddedOrRemovedCds;
					std::list<Signal<RenderingControllerPtr>::cd>			_enterFrameCds;

				private:
					void
					initialize();

					void
					targetAddedHandler(AbstractControllerPtr ctrl, NodePtr target);

					void
					controllerAddedOrRemovedHandler(NodePtr node, AbstractControllerPtr	ctrl);

					void
					descendantAddedOrRemovedHandler(NodePtr node, NodePtr descendant);

					void
					updateEnterFrameListeners();

					void
					updateControllerAddedOrRemovedListeners();

					void
					enterFrameHandler(std::shared_ptr<RenderingController> ctrl);
				};
			};
		}
	}
}
