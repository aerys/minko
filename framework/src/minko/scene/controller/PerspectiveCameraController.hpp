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
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/scene/data/DataProvider.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/scene/data/DataBindings.hpp"

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
				typedef std::shared_ptr<PerspectiveCameraController> Ptr;

			private:
				typedef std::shared_ptr<AbstractController>	AbsCtrlPtr;
				typedef std::shared_ptr<Node>				NodePtr;

			private:
				std::shared_ptr<Matrix4x4>				_view;
				std::shared_ptr<Matrix4x4>				_projection;
				std::shared_ptr<Matrix4x4>				_viewProjection;

				std::shared_ptr<DataProvider>			_data;

				Signal<AbsCtrlPtr, NodePtr>::Slot			_targetAddedSlot;
				Signal<AbsCtrlPtr, NodePtr>::Slot			_targetRemovedSlot;
				DataBindings::PropertyChangedSignal::Slot	_modelToWorldChangedSlot;

			public:
				inline static
				Ptr
				create(float fov, float aspectRatio, float zNear, float zFar)
				{
					auto ctrl  = std::shared_ptr<PerspectiveCameraController>(new PerspectiveCameraController(
						fov, aspectRatio, zNear, zFar
					));

					ctrl->initialize();

					return ctrl;
				}

			private:
				PerspectiveCameraController(float fov, float aspectRatio, float zNear, float zFar);

				void
				initialize();

				void
				targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> node);

				void
				targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> node);

				void
				localToWorldChangedHandler(std::shared_ptr<DataBindings> bindings, const std::string& propertyName);
			};
		}
	}
}
