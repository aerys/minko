/*
Copyright (c) 2014 Aerys

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
#include "minko/data/Store.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
	namespace component
	{
		class PerspectiveCamera :
            public AbstractComponent
		{
		public:
			typedef std::shared_ptr<PerspectiveCamera> Ptr;

		private:
			typedef std::shared_ptr<AbstractComponent>	AbsCtrlPtr;
			typedef std::shared_ptr<scene::Node>		NodePtr;
            typedef std::shared_ptr<scene::NodeSet>     NodeSetPtr;

		private:
			std::shared_ptr<data::Provider>		            _data;
			float											_fov;
			float											_aspectRatio;
			float											_zNear;
			float											_zFar;

			math::mat4										_view;
			math::mat4										_projection;
			math::mat4										_viewProjection;
            math::vec3	                 					_position;
            math::vec3	                 					_direction;
			math::mat4										_postProjection;

			Signal<AbsCtrlPtr, NodePtr>::Slot				_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot				_targetRemovedSlot;
			data::Store::PropertyChangedSignal::Slot	    _modelToWorldChangedSlot;

		public:
			inline static
			Ptr
			create(float 				aspectRatio,
				   float 				fov				= .785f,
				   float 				zNear			= 0.1f,
				   float 				zFar			= 1000.f,
				   const math::mat4& 	postProjection 	= math::mat4(1.f))
			{
                return std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera(
                    fov, aspectRatio, zNear, zFar, postProjection
                ));
			}

            // TODO #Clone
            /*
            AbstractComponent::Ptr
            clone(const CloneOption& option);
			*/

			inline
			float
			fieldOfView()
			{
				return _fov;
			}

			inline
			void
			fieldOfView(float fov)
			{
				if (fov != _fov)
				{
					_fov = fov;
					updateProjection(_fov, _aspectRatio, _zNear, _zFar);
				}
			}

			inline
			float
			aspectRatio()
			{
				return _aspectRatio;
			}

			inline
			void
			aspectRatio(float aspectRatio)
			{
				if (aspectRatio != _aspectRatio)
				{
					_aspectRatio = aspectRatio;
					updateProjection(_fov, _aspectRatio, _zNear, _zFar);
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
					updateProjection(_fov, _aspectRatio, _zNear, _zFar);
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
					updateProjection(_fov, _aspectRatio, _zNear, _zFar);
				}
			}

			inline
			std::shared_ptr<data::Provider>
			data()
			{
				return _data;
			}

			inline
			const math::mat4&
			viewMatrix()
			{
				return _view;
			}

			inline
			const math::mat4&
			projectionMatrix()
			{
				return _projection;
			}

			inline
			const math::mat4&
			viewProjectionMatrix()
			{
				return _viewProjection;
			}

			~PerspectiveCamera()
			{
			}

			void
			updateProjection(float fov, float aspectRatio, float zNear, float zFar);

			std::shared_ptr<math::Ray>
			unproject(float x, float y);

			math::vec3
			project(const math::vec3& worldPosition) const;

            static
            math::vec3
            project(const math::vec3&   worldPosition,
                    unsigned int        viewportWidth,
                    unsigned int        viewportHeight,
                    const math::mat4&   viewMatrix,
                    const math::mat4&   viewProjectionMatrix);

        protected:
            void
            targetAdded(NodePtr target);

			void
			targetRemoved(NodePtr target);

		private:
			PerspectiveCamera(float				fov,
							  float				aspectRatio,
							  float				zNear,
							  float				zFar,
							  const math::mat4&	postPerspective);

            PerspectiveCamera(const PerspectiveCamera& camera, const CloneOption& option);

			void
			localToWorldChangedHandler(data::Store& data);

            void
            updateMatrices(const math::mat4& modelToWorldMatrix);

		};
	}
}
