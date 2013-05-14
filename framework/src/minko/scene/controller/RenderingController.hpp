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
				typedef std::shared_ptr<Node>	NodePtr;

			private:
				std::shared_ptr<OpenGLESContext>		_context;
				std::list<std::shared_ptr<DrawCall>>	_drawCalls;

				Signal<ptr>::ptr						_enterFrame;
				Signal<ptr>::ptr						_exitFrame;

				Signal<NodePtr, NodePtr, NodePtr>::cd	_addedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd	_removedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd	_rootAddedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd	_rootDescendantAddedCd;
				Signal<NodePtr, NodePtr, NodePtr>::cd	_rootDescendantRemovedCd;

			public:
				static
				ptr
				create(std::shared_ptr<OpenGLESContext> context)
				{
					return std::shared_ptr<RenderingController>(new RenderingController(context));
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
				RenderingController(std::shared_ptr<OpenGLESContext> context) :
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
				rootDescendantAddedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				rootDescendantRemovedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				geometryChanged(std::shared_ptr<SurfaceController>);

				void
				materialChanged(std::shared_ptr<SurfaceController>);
			};
		}
	}
}
		