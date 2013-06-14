#pragma once

#include "minko/Common.hpp"
#include "minko/scene/controller/AbstractController.hpp"

namespace
{
	using namespace minko::scene;
	using namespace minko::render;
	using namespace minko::render::context;
}

namespace minko
{
	namespace scene
	{
		namespace controller
		{
			class RenderingController :
				public AbstractController,
				public std::enable_shared_from_this<RenderingController>
			{
			public:
				typedef std::shared_ptr<RenderingController>	ptr;

			private:
				typedef std::shared_ptr<Node>				NodePtr;
				typedef std::shared_ptr<AbstractController>	AbsCtrlPtr;
				typedef std::shared_ptr<SurfaceController>	SurfaceCtrlPtr;
				typedef std::shared_ptr<DrawCall>			DrawCallPtr;

			private:
				std::shared_ptr<AbstractContext>			_context;
				std::list<std::shared_ptr<DrawCall>>		_drawCalls;

				Signal<ptr>::ptr							_enterFrame;
				Signal<ptr>::ptr							_exitFrame;

				Signal<AbsCtrlPtr, NodePtr>::cd				_targetAddedCd;
				Signal<AbsCtrlPtr, NodePtr>::cd				_targetRemovedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd		_addedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd		_removedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd		_rootDescendantAddedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd		_rootDescendantRemovedCd;
				Signal<NodePtr, NodePtr, AbsCtrlPtr>::cd	_controllerAddedCd;
				Signal<NodePtr, NodePtr, AbsCtrlPtr>::cd	_controllerRemovedCd;

			public:
				static
				ptr
				create(std::shared_ptr<AbstractContext> context)
				{
					auto ctrl = std::shared_ptr<RenderingController>(new RenderingController(context));

					ctrl->initialize();

					return ctrl;
				}

				void
				render();

				inline
				Signal<ptr>::ptr
				enterFrame()
				{
					return _enterFrame;
				}

				inline
				Signal<ptr>::ptr
				exitFrame()
				{
					return _exitFrame;
				}

			private:
				RenderingController(std::shared_ptr<AbstractContext> context) :
					AbstractController(),
					_context(context),
					_enterFrame(Signal<ptr>::create()),
					_exitFrame(Signal<ptr>::create())
				{
				}

				void
				initialize();

				void
				targetAddedHandler(std::shared_ptr<AbstractController>, NodePtr target);

				void
				targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, NodePtr target);

				void
				addedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				removedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				rootDescendantAddedHandler(NodePtr	node,
										   NodePtr	target,
										   NodePtr	parent);

				void
				rootDescendantRemovedHandler(NodePtr	node,
											 NodePtr	target,
											 NodePtr	parent);
				void
				controllerAddedHandler(NodePtr								node,
									   NodePtr								target,
									   std::shared_ptr<AbstractController>	ctrl);

				void
				controllerRemovedHandler(NodePtr								node,
										 NodePtr								target,
										 std::shared_ptr<AbstractController>	ctrl);

				void
				addSurfaceController(std::shared_ptr<SurfaceController> ctrl);

				void
				removeSurfaceController(std::shared_ptr<SurfaceController> ctrl);

				void
				geometryChanged(std::shared_ptr<SurfaceController>);

				void
				materialChanged(std::shared_ptr<SurfaceController>);
			};
		}
	}
}
