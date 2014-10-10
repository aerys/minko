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

#include "minko/Minko.hpp"
#include "minko/OculusCommon.hpp"
#include "minko/Signal.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/PerspectiveCamera.hpp"

namespace minko
{
    namespace component
    {
        class OculusVRCamera :
            public AbstractComponent
        {
        public:
            typedef std::shared_ptr<OculusVRCamera> Ptr;

        private:
            typedef std::shared_ptr<scene::Node>				NodePtr;
            typedef std::shared_ptr<AbstractComponent>			AbsCmpPtr;
            typedef std::shared_ptr<SceneManager>				SceneMgrPtr;
            typedef std::shared_ptr<render::AbstractTexture>    AbsTexturePtr;
            typedef math::Vector2::Ptr                          Vector2Ptr;

        private:
            float                                               _aspectRatio;
            float                                               _zNear;
            float                                               _zFar;

            std::shared_ptr<math::Vector3>                      _eyePosition;
            std::shared_ptr<math::Matrix4x4>                    _eyeOrientation;

            SceneMgrPtr                                         _sceneManager;
            uint                                                _renderTargetWidth;
            uint                                                _renderTargetHeight;
            std::shared_ptr<render::Texture>                    _renderTarget;
            std::shared_ptr<scene::Node>                        _leftCameraNode;
            std::shared_ptr<Renderer>                           _leftRenderer;
            std::shared_ptr<scene::Node>                        _rightCameraNode;
            std::shared_ptr<Renderer>                           _rightRenderer;
            
            scene::Node::Ptr                                    _ppScene;
            Renderer::Ptr                                       _ppRenderer;

            Signal<AbsCmpPtr, NodePtr>::Slot                    _targetAddedSlot;
            Signal<AbsCmpPtr, NodePtr>::Slot                    _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot             _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot             _removedSlot;
            Signal<SceneMgrPtr, uint, AbsTexturePtr>::Slot      _renderBeginSlot;
            Signal<SceneMgrPtr, uint, AbsTexturePtr>::Slot      _renderEndSlot;

            std::array<std::pair<Vector2Ptr, Vector2Ptr>, 2>    _uvScaleOffset;

            std::shared_ptr<oculus::OculusImpl>                 _oculusImpl;
            oculus::EyeFOV                                      _defaultLeftEyeFov;
            oculus::EyeFOV                                      _defaultRightEyeFov;

        public:
            inline static
            Ptr
            create(int viewportWidth,
                   int viewportHeight,
                   float zNear  = 0.1f,
                   float zFar   = 1000.0f)
            {
                auto ptr = std::shared_ptr<OculusVRCamera>(new OculusVRCamera(
                    viewportWidth, viewportHeight, zNear, zFar
                ));

                ptr->initialize();

                return ptr;
            }

            inline
            float
            aspectRatio() const
            {
                return _aspectRatio;
            }

            inline
            float
            zNear() const
            {
                return _zNear;
            }

            inline
            float
            zFar() const
            {
                return _zFar;
            }

            void
            updateViewport(int viewportWidth, int viewportHeight);

        public:
            ~OculusVRCamera(); // temporary solution

        private:
            OculusVRCamera(int viewportWidth, int viewportHeight, float zNear, float zFar);

            void
            resetOVRDevice();

            void
            initializeOVRDevice();

            void
            initialize();

            void
            initializeCameras();

            std::array<std::shared_ptr<geometry::Geometry>, 2>
            createDistortionGeometry(std::shared_ptr<render::AbstractContext> context);

            void
            initializePostProcessingRenderer();

            void
            updateCameraOrientation();

            void
            targetAddedHandler(AbsCmpPtr component, NodePtr target);

            void
            targetRemovedHandler(AbsCmpPtr component, NodePtr target);

            void
            renderEndHandler(SceneMgrPtr sceneManager, uint frameId, AbsTexturePtr renderTarget);

            void
            findSceneManager();

            void
            setSceneManager(SceneMgrPtr);
        };
    }
}

