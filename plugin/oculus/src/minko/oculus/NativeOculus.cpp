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

#include "OVR_CAPI.h"
#include "CAPI/CAPI_HMDState.h"
#include "OVR_Stereo.h"
#include "Kernel/OVR_Math.h"

#include "minko/oculus/NativeOculus.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;

void
NativeOculus::initializeOVRDevice(
    Renderer::Ptr leftRenderer, 
    Renderer::Ptr rightRenderer,
    uint& renderTargetWidth,
    uint& renderTargetHeight,
    std::array<std::pair<Vector2Ptr, Vector2Ptr>, 2>& uvScaleOffset)
{
    ovr_Initialize();

    _hmd = ovrHmd_Create(0);
    if (!_hmd)
        throw;

    ovrHmd_RecenterPose(_hmd);

    OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(_hmd, ovrEye_Left, _hmd->DefaultEyeFov[0], 1.0f);
    OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(_hmd, ovrEye_Right, _hmd->DefaultEyeFov[1], 1.0f);
    OVR::Sizei renderTargetSize;
    renderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
    renderTargetSize.h = std::max(recommenedTex0Size.h, recommenedTex1Size.h);

    // enforce a power of 2 size because that's what minko expects
    renderTargetSize.w = math::clp2(renderTargetSize.w);
    renderTargetSize.h = math::clp2(renderTargetSize.h);

    renderTargetSize.w = std::min(renderTargetSize.w, MINKO_PLUGIN_OCULUS_MAX_TARGET_SIZE);
    renderTargetSize.h = std::min(renderTargetSize.h, MINKO_PLUGIN_OCULUS_MAX_TARGET_SIZE);

    renderTargetWidth = renderTargetSize.w;
    renderTargetHeight = renderTargetSize.h;

    // compute each viewport pos and size
    ovrRecti eyeRenderViewport[2];
    ovrFovPort eyeFov[2] = { 
        _hmd->DefaultEyeFov[0], 
        _hmd->DefaultEyeFov[1] 
    };

    eyeRenderViewport[0].Pos = OVR::Vector2i(0, 0);
    eyeRenderViewport[0].Size = OVR::Sizei(renderTargetSize.w / 2, renderTargetSize.h);
    eyeRenderViewport[1].Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
    eyeRenderViewport[1].Size = eyeRenderViewport[0].Size;

    // create 1 renderer/eye and init. their respective viewport
    
    leftRenderer->viewport(
        eyeRenderViewport[0].Pos.x,
        eyeRenderViewport[0].Pos.y,
        eyeRenderViewport[0].Size.w,
        eyeRenderViewport[0].Size.h
    );

   
    rightRenderer->viewport(
        eyeRenderViewport[1].Pos.x,
        eyeRenderViewport[1].Pos.y,
        eyeRenderViewport[1].Size.w,
        eyeRenderViewport[1].Size.h
    );

    ovrHmd_SetEnabledCaps(_hmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

    // Start the sensor which informs of the Rift's pose and motion
    ovrHmd_ConfigureTracking(
        _hmd,
        ovrTrackingCap_Orientation |
        ovrTrackingCap_MagYawCorrection |
        ovrTrackingCap_Position,
        0
    );

    for (auto eyeNum = 0u; eyeNum < 2; ++eyeNum)
    {
        ovrVector2f ovrUVScaleOffset[2];

        ovrHmd_GetRenderScaleAndOffset(
            _hmd->DefaultEyeFov[eyeNum],
            renderTargetSize,
            eyeRenderViewport[eyeNum],
            ovrUVScaleOffset
        );

        uvScaleOffset[eyeNum].first->setTo(ovrUVScaleOffset[0].x, ovrUVScaleOffset[0].y);
        uvScaleOffset[eyeNum].second->setTo(ovrUVScaleOffset[1].x, ovrUVScaleOffset[1].y);
    }

    // FIXME: on Windows, render directly into the HMD (window ?= SDL window)
    // ovrHmd_AttachToWindow(HMD, window, NULL, NULL);
}

void
NativeOculus::destroy()
{
    ovrHmd_Destroy(_hmd);
    ovr_Shutdown();
}

std::array<std::shared_ptr<geometry::Geometry>, 2>
NativeOculus::createDistortionGeometry(std::shared_ptr<render::AbstractContext> context)
{
    auto geometries = std::array<std::shared_ptr<geometry::Geometry>, 2>();

    for (int eyeNum = 0; eyeNum < 2; eyeNum++)
    {
        auto geom = geometry::Geometry::create();

        // Allocate mesh vertices, registering with renderer using the OVR vertex format.
        ovrDistortionMesh meshData;
        ovrHmd_CreateDistortionMesh(
            _hmd,
            (ovrEyeType)eyeNum,
            _hmd->DefaultEyeFov[eyeNum],
            ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
            &meshData
            );

        auto vb = render::VertexBuffer::create(
            context,
            (float*)meshData.pVertexData,
            (sizeof(ovrDistortionVertex) / sizeof(float)) * meshData.VertexCount
         );

        // struct ovrDistortionVertex {
        //     ovrVector2f ScreenPosNDC;    // [-1,+1],[-1,+1] over the entire framebuffer.
        //     float       TimeWarpFactor;  // Lerp factor between time-warp matrices. Can be encoded in Pos.z.
        //     float       VignetteFactor;  // Vignette fade factor. Can be encoded in Pos.w.
        //     ovrVector2f TanEyeAnglesR;
        //     ovrVector2f TanEyeAnglesG;
        //     ovrVector2f TanEyeAnglesB;
        // }

        vb->addAttribute("screenPosNDC", 2);
        vb->addAttribute("timeWarpFactor", 1);
        vb->addAttribute("vignetteFactor", 1);
        vb->addAttribute("tanEyeAnglesR", 2);
        vb->addAttribute("tanEyeAnglesG", 2);
        vb->addAttribute("tanEyeAnglesB", 2);
        geom->addVertexBuffer(vb);

        auto ib = render::IndexBuffer::create(
            context,
            meshData.pIndexData,
            meshData.pIndexData + meshData.IndexCount
        );

        geom->indices(ib);

        geometries[eyeNum] = geom;
    }

    return geometries;
}

EyeFOV
NativeOculus::getDefaultLeftEyeFov()
{
    EyeFOV leftEyeFov;

    leftEyeFov.DownTan = _hmd->DefaultEyeFov[0].DownTan;
    leftEyeFov.LeftTan = _hmd->DefaultEyeFov[0].LeftTan;
    leftEyeFov.RightTan = _hmd->DefaultEyeFov[0].RightTan;
    leftEyeFov.UpTan = _hmd->DefaultEyeFov[0].UpTan;

    return leftEyeFov;
}

EyeFOV
NativeOculus::getDefaultRightEyeFov()
{
    EyeFOV rightEyeFov;

    rightEyeFov.DownTan = _hmd->DefaultEyeFov[1].DownTan;
    rightEyeFov.LeftTan = _hmd->DefaultEyeFov[1].LeftTan;
    rightEyeFov.RightTan = _hmd->DefaultEyeFov[1].RightTan;
    rightEyeFov.UpTan = _hmd->DefaultEyeFov[1].UpTan;

    return rightEyeFov;
}

void
NativeOculus::updateCameraOrientation(
    std::array<Matrix4x4::Ptr, 2> viewMatrixes, 
    scene::Node::Ptr ppScene)
{
    static ovrPosef eyeRenderPose[2];
    static float BodyYaw(0.f);
    static OVR::Vector3f HeadPos(0.0f, 0.f, 0.f);
    static auto state = (OVR::CAPI::HMDState*)(_hmd->Handle);
    static auto renderInfo = state->RenderState.RenderInfo;

    HeadPos.y = ovrHmd_GetFloat(_hmd, OVR_KEY_EYE_HEIGHT, HeadPos.y);

    for (int eyeNum = 0; eyeNum < 2; eyeNum++)
    {
        ovrEyeType eye = _hmd->EyeRenderOrder[eyeNum];
        eyeRenderPose[eye] = ovrHmd_GetEyePose(_hmd, eye);

        OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(BodyYaw);
        OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(eyeRenderPose[eye].Orientation);
        OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
        OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
        OVR::Vector3f shiftedEyePos = HeadPos + rollPitchYaw.Transform(eyeRenderPose[eye].Position);
        OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
        OVR::Vector3f viewAdjust = OVR::Util::Render::CalculateEyeVirtualCameraOffset(
            renderInfo,
            eyeNum == 0 ? OVR::StereoEye::StereoEye_Left : OVR::StereoEye::StereoEye_Right,
            false
        );

        auto matrix = viewMatrixes[eyeNum];

        matrix->lock();
        matrix->initialize((float*)view.M);
        //matrix->transpose();
        matrix->invert();
        matrix->prependTranslation(viewAdjust.x, viewAdjust.y, viewAdjust.z);
        matrix->unlock();

        // update time warp matrices
        ovrMatrix4f twMatrices[2];
        ovrHmd_GetEyeTimewarpMatrices(_hmd, (ovrEyeType)eyeNum, eyeRenderPose[eyeNum], twMatrices);

        Matrix4x4::Ptr rotationStart = Matrix4x4::create()->initialize((float*)twMatrices[0].M);
        Matrix4x4::Ptr rotationEnd = Matrix4x4::create()->initialize((float*)twMatrices[1].M);

        ppScene->component<Surface>(eyeNum)->material()
            ->set("eyeRotationStart", rotationStart)
            ->set("eyeRotationEnd", rotationEnd);
    }
}