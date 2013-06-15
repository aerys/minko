#pragma once

#include "minko/Common.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/scene/data/DataProvider.hpp"
#include "minko/math/Matrix4x4.hpp"

namespace
{
	using namespace minko::math;
	using namespace minko::scene::data;
}

namespace minko
{
	namespace scene
	{
		namespace controller
		{
			class PerspectiveCameraController :
				public AbstractController,
				public std::enable_shared_from_this<PerspectiveCameraController>
			{
			public:
				typedef std::shared_ptr<PerspectiveCameraController> ptr;

			private:
				typedef std::shared_ptr<AbstractController>	AbsCtrlPtr;
				typedef std::shared_ptr<Node>				NodePtr;

			private:
				std::shared_ptr<Matrix4x4>		_view;
				std::shared_ptr<Matrix4x4>		_projection;
				std::shared_ptr<Matrix4x4>		_viewProjection;

				std::shared_ptr<DataProvider>	_data;

				Signal<AbsCtrlPtr, NodePtr>::cd	_targetAddedCd;
				Signal<AbsCtrlPtr, NodePtr>::cd	_targetRemovedCd;

			public:
				inline static
				ptr
				create(float fov, float aspectRatio, float zNear, float zFar)
				{
					auto ctrl  = new PerspectiveCameraController(
						fov, aspectRatio, zNear, zFar
					);

					ctrl->initialize();

					return std::shared_ptr<PerspectiveCameraController>(ctrl);
				}

			private:
				PerspectiveCameraController(float fov, float aspectRatio, float zNear, float zFar);

				void
				initialize();

				void
				targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> node);

				void
				targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> node);

			};
		}
	}
}
