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
#include "minko/render/AbstractContext.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/Texture.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;
using namespace minko::render;
using namespace minko::file;

ovrHmd
NativeOculus::_hmd = nullptr;

NativeOculus::NativeOculus(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _ppRenderer(Renderer::create()),
    _zNear(zNear),
    _zFar(zFar),
    _aspectRatio((float)viewportWidth / (float)viewportHeight),
    _leftRenderer(nullptr),
    _rightRenderer(nullptr),
    _renderEndSlot(nullptr),
    _leftCameraNode(nullptr),
    _rightCameraNode(nullptr)
{
    _uvScaleOffset[0].first = math::Vector2::create();
    _uvScaleOffset[0].second = math::Vector2::create();
    _uvScaleOffset[1].first = math::Vector2::create();
    _uvScaleOffset[1].second = math::Vector2::create();
}

void
NativeOculus::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
    _sceneManager = sceneManager;

    _renderEndSlot = sceneManager->renderingEnd()->connect(std::bind(
        &NativeOculus::renderEndHandler,
        std::static_pointer_cast<NativeOculus>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
        )
    );

    initializePostProcessingRenderer();
}

bool
NativeOculus::detected()
{
    if (_hmd)
        return true;

     ovr_Initialize();

    _hmd = ovrHmd_Create(0);

    return _hmd != nullptr;
}

void
NativeOculus::initializeOVRDevice()
{
    _leftRenderer = Renderer::create();
    _rightRenderer = Renderer::create();

    // Create renderer for each eye
    _rightRenderer->clearBeforeRender(false);

    ovr_Initialize();

    _hmd = ovrHmd_Create(0);

    if (!_hmd)
        return;

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

    _renderTargetWidth = renderTargetSize.w;
    _renderTargetHeight = renderTargetSize.h;

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
    
    _leftRenderer->viewport(
        eyeRenderViewport[0].Pos.x,
        eyeRenderViewport[0].Pos.y,
        eyeRenderViewport[0].Size.w,
        eyeRenderViewport[0].Size.h
    );

   
    _rightRenderer->viewport(
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

        _uvScaleOffset[eyeNum].first->setTo(ovrUVScaleOffset[0].x, ovrUVScaleOffset[0].y);
        _uvScaleOffset[eyeNum].second->setTo(ovrUVScaleOffset[1].x, ovrUVScaleOffset[1].y);
    }

    // FIXME: on Windows, render directly into the HMD (window ?= SDL window)
    //ovrHmd_AttachToWindow(_hmd, window, NULL, NULL);
}

void
NativeOculus::initializeCameras(scene::Node::Ptr target)
{
    auto aspectRatio = (float)_renderTargetWidth / (float)_renderTargetHeight;

    auto leftCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(_hmd->DefaultEyeFov[0].LeftTan + _hmd->DefaultEyeFov[0].RightTan),
        _zNear,
        _zFar
    );

    _leftCameraNode = scene::Node::create("oculusLeftEye")
        ->addComponent(Transform::create())
        ->addComponent(leftCamera)
        ->addComponent(_leftRenderer);

    target->addChild(_leftCameraNode);

    auto rightCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(_hmd->DefaultEyeFov[1].LeftTan + _hmd->DefaultEyeFov[1].RightTan),
        _zNear,
        _zFar
        );

    _rightCameraNode = scene::Node::create("oculusRightEye")
        ->addComponent(Transform::create())
        ->addComponent(rightCamera)
        ->addComponent(_rightRenderer);

    target->addChild(_rightCameraNode);
}

void
NativeOculus::initializePostProcessingRenderer()
{
    auto context = _sceneManager->assets()->context();

    _renderTarget = render::Texture::create(context, _renderTargetWidth, _renderTargetHeight, false, true);
    _renderTarget->upload();
    _leftRenderer->target(_renderTarget);
    _rightRenderer->target(_renderTarget);

    auto geometries = createDistortionGeometry(context);
    auto loader = file::Loader::create(_sceneManager->assets()->loader());

    loader->queue("effect/OculusVR/OculusVR.effect");

    auto complete = loader->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto effect = _sceneManager->assets()->effect("effect/OculusVR/OculusVR.effect");

        auto materialLeftEye = material::Material::create();
        materialLeftEye->set("eyeToSourceUVScale", _uvScaleOffset[0].first);
        materialLeftEye->set("eyeToSourceUVOffset", _uvScaleOffset[0].second);
        materialLeftEye->set("eyeRotationStart", math::Matrix4x4::create());
        materialLeftEye->set("eyeRotationEnd", math::Matrix4x4::create());
        materialLeftEye->set("texture", _renderTarget);

        auto materialRightEye = material::Material::create();
        materialRightEye->set("eyeToSourceUVScale", _uvScaleOffset[1].first);
        materialRightEye->set("eyeToSourceUVOffset", _uvScaleOffset[1].second);
        materialRightEye->set("eyeRotationStart", math::Matrix4x4::create());
        materialRightEye->set("eyeRotationEnd", math::Matrix4x4::create());
        materialRightEye->set("texture", _renderTarget);

        _ppScene = scene::Node::create()
            ->addComponent(_ppRenderer)
            ->addComponent(Surface::create(geometries[0], materialLeftEye, effect))
            ->addComponent(Surface::create(geometries[1], materialRightEye, effect));
    });

    loader->load();
}

void
NativeOculus::destroy()
{
    ovrHmd_Destroy(_hmd);
    ovr_Shutdown();
}

void
NativeOculus::updateViewport(int viewportWidth, int viewportHeight)
{
    _aspectRatio = (float)viewportWidth / (float)viewportHeight;
    _ppRenderer->viewport(0, 0, viewportWidth, viewportHeight);

    /*if (_leftCameraNode)
    _leftCameraNode->component<PerspectiveCamera>()->aspectRatio(_aspectRatio);
    if (_rightCameraNode)
    _rightCameraNode->component<PerspectiveCamera>()->aspectRatio(_aspectRatio);*/
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
NativeOculus::updateCameraOrientation(std::shared_ptr<scene::Node> target)
{
    static ovrPosef eyeRenderPose[2];
    const static float BodyYaw(0.f);
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

        auto cameraNode = eyeNum == 0 ? _leftCameraNode : _rightCameraNode;
        auto matrix = cameraNode->component<Transform>()->matrix();

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

        _ppScene->component<Surface>(eyeNum)->material()
            ->set("eyeRotationStart", rotationStart)
            ->set("eyeRotationEnd", rotationEnd);
    }
}

void
NativeOculus::renderEndHandler(
    std::shared_ptr<SceneManager>  sceneManager,
    uint                           frameId,
    render::AbstractTexture::Ptr   renderTarget)
{
    _ppRenderer->render(sceneManager->assets()->context());
}