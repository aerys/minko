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

#include "minko/component/AbstractComponent.hpp"
#include "minko/data/Provider.hpp"

namespace player
{
    namespace component
    {
        class ClippingPlane :
            public minko::component::AbstractComponent
        {
        public:
            typedef std::shared_ptr<ClippingPlane>                  Ptr;

            typedef std::shared_ptr<minko::scene::Node>             NodePtr;
            typedef std::function<NodePtr(NodePtr)>                 NodeFunction;

        private:
            typedef minko::Signal<NodePtr, NodePtr, NodePtr>::Slot  NodeSignalSlot;

            typedef std::shared_ptr<minko::data::Provider>          ProviderPtr;

            typedef minko::Signal<
                minko::data::Store&,
                ProviderPtr,
                const minko::data::Provider::PropertyName&
            >::Slot                                                 PropertyChangedSlot;

            typedef std::shared_ptr<minko::component::Renderer>     RendererPtr;
            typedef std::shared_ptr<minko::component::SceneManager> SceneManagerPtr;

            typedef std::shared_ptr<minko::geometry::Geometry>      GeometryPtr;
            typedef std::shared_ptr<minko::material::Material>      MaterialPtr;
            typedef std::shared_ptr<minko::render::Effect>          EffectPtr;

        private:
            SceneManagerPtr     _sceneManager;

            NodeSignalSlot      _addedSlot;

            int                 _planeId;
            NodePtr             _originNode;
            NodePtr             _planeNode;

            minko::math::mat4   _basePlaneTransformMatrix;

            static int                 _nextPlaneId;
            static RendererPtr         _depthRenderer;
            static RendererPtr         _stencilRenderer;

            GeometryPtr         _planeGeometry;
            MaterialPtr         _planeMaterial;
            EffectPtr           _planeEffect;

            ProviderPtr         _provider;

            PropertyChangedSlot _planeNodeModelToWorldChangedSlot;

            NodeFunction        _cameraNodeFunction;

        public:
            inline
            static
            Ptr
            create()
            {
                return Ptr(new ClippingPlane());
            }

            inline
            void
            cameraNodeFunction(NodeFunction cameraNodeFunction)
            {
                _cameraNodeFunction = cameraNodeFunction;
            }

            inline
            NodePtr
            originNode() const
            {
                return _originNode;
            }

            inline
            void
            basePlaneTransformMatrix(const minko::math::mat4& matrix)
            {
                _basePlaneTransformMatrix = matrix;
            }

            void
            enable(bool enabled);

        protected:
            void
            targetAdded(NodePtr target);

            void
            targetRemoved(NodePtr target);

        private:
            ClippingPlane();

            void
            initialize();

            void
            addedHandler(NodePtr node,
                         NodePtr target,
                         NodePtr added);

            std::string
            stringifiedPlaneId()
            {
                return "clippingPlane" + std::to_string(_planeId);
            }

            GeometryPtr
            createDefaultPlaneGeometry();

            MaterialPtr
            createDefaultPlaneMaterial();

            EffectPtr
            createDefaultPlaneEffect();

            void
            createRenderers(NodePtr cameraNode);

            void
            transformChanged(const minko::math::mat4& matrix);

            void
            updatePlane(const minko::math::vec3& position, const minko::math::vec3& normal);

            void
            setPlaneData(const minko::math::vec4& plane);
        };
    }
}
