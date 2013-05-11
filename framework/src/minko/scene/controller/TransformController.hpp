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
				std::shared_ptr<Matrix4x4>			_transform;
				std::shared_ptr<Matrix4x4>			_modelToWorld;
				std::shared_ptr<DataProvider>		_data;

				NodePtr								_referenceFrame;

				Signal<std::shared_ptr<Scene>>::cd	_enterFrameCd;

			private:
				TransformController();

				void
				initialize();

				void
				targetAddedHandler(std::shared_ptr<AbstractController> ctrl, NodePtr target);

				void
				targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, NodePtr target);

				void
				addedHandler(NodePtr node, NodePtr ancestor);

				void
				removedHandler(NodePtr node, NodePtr ancestor);

				void
				enterFrameHandler(std::shared_ptr<Scene> scene);

				void
				updateReferenceFrame(NodePtr node);
			};			
		}
	}
}
