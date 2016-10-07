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
            typedef std::shared_ptr<AbstractComponent>  AbsCtrlPtr;
            typedef std::shared_ptr<scene::Node>        NodePtr;
            typedef std::shared_ptr<scene::NodeSet>     NodeSetPtr;

        private:
            std::shared_ptr<data::Provider>                 _data;

            math::mat4                                      _view;
            math::mat4                                      _projection;
            math::mat4                                      _viewProjection;
            math::vec3                                      _position;
            math::vec3                                      _direction;
            math::mat4                                      _postProjection;

            Signal<AbsCtrlPtr, NodePtr>::Slot               _targetAddedSlot;
            Signal<AbsCtrlPtr, NodePtr>::Slot               _targetRemovedSlot;
            data::Store::PropertyChangedSignal::Slot        _modelToWorldChangedSlot;

        public:
            inline static
            Ptr
            create(const math::mat4&    projection,
                   const math::mat4&    postProjection  = math::mat4(1.f))
            {
                return std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera(
                    projection, postProjection
                ));
            }

            // TODO #Clone
            /*
            AbstractComponent::Ptr
            clone(const CloneOption& option);
            */

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

            inline
            void
            projectionMatrix(const math::mat4& projection)
            {
                _projection = projection;
                updateWorldToScreenMatrix();
            }

            ~PerspectiveCamera()
            {
            }

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
            PerspectiveCamera(const math::mat4& projection,
                              const math::mat4& postProjection);

            PerspectiveCamera(const PerspectiveCamera& camera, const CloneOption& option);

            void
            localToWorldChangedHandler(data::Store& data);

            void
            updateMatrices(const math::mat4& modelToWorldMatrix);

            void
            updateWorldToScreenMatrix();
        };
    }
}
