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

#include "minko/component/AbstractComponent.hpp"
#include "minko/data/Provider.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/data/Container.hpp"

namespace minko
{
	namespace component
	{
		class PerspectiveCamera :
			public AbstractComponent,
			public std::enable_shared_from_this<PerspectiveCamera>
		{
		public:
			typedef std::shared_ptr<PerspectiveCamera> Ptr;

		private:
			typedef std::shared_ptr<AbstractComponent>	AbsCtrlPtr;
			typedef std::shared_ptr<scene::Node>		NodePtr;
            typedef std::shared_ptr<scene::NodeSet>     NodeSetPtr;

		private:
            bool                                            _enabled;

			std::shared_ptr<math::Matrix4x4>				_view;
			std::shared_ptr<math::Matrix4x4>				_projection;
			std::shared_ptr<math::Matrix4x4>				_viewProjection;

			std::shared_ptr<data::Provider>					_data;

			Signal<AbsCtrlPtr, NodePtr>::Slot				_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot				_targetRemovedSlot;
			data::Container::PropertyChangedSignal::Slot	_modelToWorldChangedSlot;

		public:
			inline static
			Ptr
			create(float fov, float aspectRatio, float zNear, float zFar)
			{
				auto ctrl  = std::shared_ptr<PerspectiveCamera>(
                    new PerspectiveCamera(fov, aspectRatio, zNear, zFar)
                );

				ctrl->initialize();

				return ctrl;
			}

		private:
			PerspectiveCamera(float fov, float aspectRatio, float zNear, float zFar);

			void
			initialize();

			void
			targetAddedHandler(AbsCtrlPtr ctrl, NodePtr node);

			void
			targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr node);

			void
			localToWorldChangedHandler(std::shared_ptr<data::Container> data,
									   const std::string&				propertyName);
		};
	}
}
