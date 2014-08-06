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

#include "minko/Signal.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/PerspectiveCamera.hpp"

namespace OVR
{
    class System;
    class HMDDevice;
    class SensorDevice;
    class SensorFusion;
}

namespace minko
{
    namespace component
    {
        class OculusVRCamera :
            public AbstractComponent
        {
        public:
            typedef std::shared_ptr<OculusVRCamera>    Ptr;

        private:
            struct HMDInfo
            {
                float hResolution;
                float vResolution;
                float hScreenSize;
                float vScreenSize;
                float vScreenCenter;
                float interpupillaryDistance;
                float lensSeparationDistance;
                float eyeToScreenDistance;
                std::shared_ptr<math::Vector4> distortionK;
            };

        private:
            typedef std::shared_ptr<scene::Node>                NodePtr;
            typedef std::shared_ptr<AbstractComponent>            AbsCmpPtr;
            typedef std::shared_ptr<SceneManager>                SceneMgrPtr;
            typedef std::shared_ptr<render::AbstractTexture>    AbsTexturePtr;

        private:
            static const float                                    WORLD_UNIT;
            static const unsigned int                            TARGET_SIZE;

            float                                                _aspectRatio;
            float                                                _zNear;
            float                                                _zFar;
            HMDInfo                                                _hmdInfo;

            std::shared_ptr<OVR::System>                        _ovrSystem;
            std::shared_ptr<OVR::HMDDevice>                        _ovrHMDDevice;
            std::shared_ptr<OVR::SensorDevice>                    _ovrSensorDevice;
            std::shared_ptr<OVR::SensorFusion>                    _ovrSensorFusion;

            std::shared_ptr<Transform>                            _targetTransform;
            std::shared_ptr<math::Vector3>                        _eyePosition;
            std::shared_ptr<math::Matrix4x4>                    _eyeOrientation;

            SceneMgrPtr                                            _sceneManager;
            NodePtr                                                _root;
            std::shared_ptr<PerspectiveCamera>                    _leftCamera;
            std::shared_ptr<PerspectiveCamera>                    _rightCamera;
            std::shared_ptr<Renderer>                            _renderer;

            Signal<AbsCmpPtr, NodePtr>::Slot                    _targetAddedSlot;
            Signal<AbsCmpPtr, NodePtr>::Slot                    _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot                _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot                _removedSlot;
            Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot            _targetComponentAddedHandler;
            Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot            _targetComponentRemovedHandler;
            Signal<SceneMgrPtr, uint, AbsTexturePtr>::Slot        _renderEndSlot;

        public:
            inline static
            Ptr
            create(float aspectRatio,
                   float zNear            = 0.1f,
                   float zFar            = 1000.0f)
            {
                auto ptr = std::shared_ptr<OculusVRCamera>(new OculusVRCamera(aspectRatio, zNear, zFar));

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
            resetHeadTracking();

            bool
            HMDDeviceDetected() const;

            bool
            sensorDeviceDetected() const;

        public:
            ~OculusVRCamera(); // temporary solution

        private:
            OculusVRCamera(float aspectRatio, float zNear, float zFar);

            void
            resetOVRDevice();

            void
            initializeOVRDevice();

            void
            initialize();

            void
            updateCameraOrientation();

            void
            targetAddedHandler(AbsCmpPtr component, NodePtr target);

            void
            targetRemovedHandler(AbsCmpPtr component, NodePtr target);

            void
            addedHandler(NodePtr, NodePtr, NodePtr);

            void
            removedHandler(NodePtr, NodePtr, NodePtr);

            void
            renderEndHandler(SceneMgrPtr sceneManager, uint frameId, AbsTexturePtr    renderTarget);

            void
            targetComponentAddedHandler(NodePtr, NodePtr, AbsCmpPtr);

            void
            targetComponentRemovedHandler(NodePtr, NodePtr, AbsCmpPtr);

            void
            findSceneManager();

            void
            setSceneManager(SceneMgrPtr);

            void
            getHMDInfo(HMDInfo&) const;

            static
            float
            distort(float, std::shared_ptr<math::Vector4>);
        };
    }
}

