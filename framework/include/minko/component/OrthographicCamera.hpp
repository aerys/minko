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

//#include "minko/component/AbstractRootDataComponent.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/Container.hpp"
#include "minko/Signal.hpp"

namespace minko
{
	namespace component
	{
		class OrthographicCamera :
            public AbstractComponent,
			public std::enable_shared_from_this<OrthographicCamera>
		{
		public:
			typedef std::shared_ptr<OrthographicCamera> Ptr;

		private:
			typedef std::shared_ptr<AbstractComponent>	AbsCtrlPtr;
			typedef std::shared_ptr<scene::Node>		NodePtr;
            typedef std::shared_ptr<scene::NodeSet>     NodeSetPtr;

		private:
			std::shared_ptr<data::StructureProvider>		_data;
			float											_left;
			float											_right;
			float											_top;
			float											_bottom;
			float											_zNear;
			float											_zFar;

			std::shared_ptr<math::Matrix4x4>				_view;
			std::shared_ptr<math::Matrix4x4>				_projection;
			std::shared_ptr<math::Matrix4x4>				_viewProjection;
            std::shared_ptr<math::Vector3>                  _position;
			
			std::shared_ptr<math::Matrix4x4>				_postProjection;

			Signal<AbsCtrlPtr, NodePtr>::Slot				_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot				_targetRemovedSlot;
			data::Container::PropertyChangedSignal::Slot	_modelToWorldChangedSlot;

		public:
			inline static
			Ptr
			create(float left, 
				   float right,
				   float top,
				   float bottom,
				   float zNear	= 0.1f, 
				   float zFar	= 1000.f)
			{
				auto ctrl = std::shared_ptr<OrthographicCamera>(new OrthographicCamera(left, right, top, bottom, zNear, zFar));

				ctrl->initialize();

				return ctrl;
			}

			inline static
			Ptr
			create(float left,
				   float right,
				   float top,
				   float bottom,
				   float zNear, 
				   float zFar, 
				   std::shared_ptr<math::Matrix4x4> postProjection)
			{
				auto ctrl = std::shared_ptr<OrthographicCamera>(new OrthographicCamera(left, right, top, bottom, zNear, zFar, postProjection));

				ctrl->initialize();

				return ctrl;
			}

			inline
			float
			left()
			{
				return _left;
			}

			inline
			void
			left(float left)
			{
				if (left != _left)
				{
					_left = left;
					updateProjection(_left, _right, _top, _bottom, _zNear, _zFar);
				}
			}

			inline
			float
			right()
			{
				return _right;
			}

			inline
			void
			right(float right)
			{
				if (right != _right)
				{
					_right = right;
					updateProjection(_left, _right, _top, _bottom, _zNear, _zFar);
				}
			}

			inline
			float
			top()
			{
				return _top;
			}

			inline
			void
			top(float top)
			{
				if (top != _top)
				{
					_top = top;
					updateProjection(_left, _right, _top, _bottom, _zNear, _zFar);
				}
			}

			inline
			float
			bottom()
			{
				return _bottom;
			}

			inline
			void
			bottom(float bottom)
			{
				if (bottom != _bottom)
				{
					_bottom = bottom;
					updateProjection(_left, _right, _top, _bottom, _zNear, _zFar);
				}
			}

			inline
			float
			zNear()
			{
				return _zNear;
			}

			inline
			void
			zNear(float zNear)
			{
				if (zNear != _zNear)
				{
					_zNear = zNear;
					updateProjection(_left, _right, _top, _bottom, _zNear, _zFar);
				}
			}

			inline
			float
			zFar()
			{
				return _zFar;
			}

			inline
			void
			zFar(float zFar)
			{
				if (zFar != _zFar)
				{
					_zFar = zFar;
					updateProjection(_left, _right, _top, _bottom, _zNear, _zFar);
				}
			}

			inline
			std::shared_ptr<data::StructureProvider>
			data()
			{
				return _data;
			}

			~OrthographicCamera()
			{
			}

			void
			updateProjection(float left, float right, float top, float bottom, float zNear, float zFar);

			std::shared_ptr<math::Ray>
			unproject(float x, float y, std::shared_ptr<math::Ray> out = nullptr);

        protected:
            void
            targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

		private:
			OrthographicCamera(float							left,
							  float								right,
							  float								top,
							  float								bottom,
							  float								zNear,
							  float								zFar,
							  std::shared_ptr<math::Matrix4x4>	postPerspective = nullptr);

			void
			initialize();

			void
			localToWorldChangedHandler(std::shared_ptr<data::Container> data,
									   const std::string&				propertyName);

            void
            updateMatrices(std::shared_ptr<math::Matrix4x4> modelToWorldMatrix);

		};
	}
}
