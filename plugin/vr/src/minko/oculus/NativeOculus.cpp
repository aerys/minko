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
#include "minko/render/Texture.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::oculus;
using namespace minko::render;
using namespace minko::file;

ovrHmd
NativeOculus::_hmd = nullptr;

NativeOculus::NativeOculus(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _ppRenderer(Renderer::create()),
    _renderTargetWidth(32),
    _renderTargetHeight(32),
    _renderEndSlot(nullptr),
    _zNear(zNear),
    _zFar(zFar)
{
    _uvScaleOffset[0].first = math::vec2();
    _uvScaleOffset[0].second = math::vec2();
    _uvScaleOffset[1].first = math::vec2();
    _uvScaleOffset[1].second = math::vec2();
}

void
NativeOculus::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
    _renderEndSlot = sceneManager->renderingEnd()->connect(std::bind(
        &NativeOculus::renderEndHandler,
        std::static_pointer_cast<NativeOculus>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
        )
    );

    initializePostProcessingRenderer(sceneManager);
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
NativeOculus::initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window)
{
    _leftRenderer = leftRenderer;
    _rightRenderer = rightRenderer;

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

    // Enforce a power of 2 size because that's what minko expects
    renderTargetSize.w = math::clp2(renderTargetSize.w);
    renderTargetSize.h = math::clp2(renderTargetSize.h);

    renderTargetSize.w = std::min(renderTargetSize.w, MINKO_PLUGIN_OCULUS_MAX_TARGET_SIZE);
    renderTargetSize.h = std::min(renderTargetSize.h, MINKO_PLUGIN_OCULUS_MAX_TARGET_SIZE);

    _renderTargetWidth = renderTargetSize.w;
    _renderTargetHeight = renderTargetSize.h;

    // Compute each viewport pos and size
    ovrRecti eyeRenderViewport[2];
    ovrFovPort eyeFov[2] = {
        _hmd->DefaultEyeFov[0],
        _hmd->DefaultEyeFov[1]
    };

    eyeRenderViewport[0].Pos = OVR::Vector2i(0, 0);
    eyeRenderViewport[0].Size = OVR::Sizei(renderTargetSize.w / 2, renderTargetSize.h);
    eyeRenderViewport[1].Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
    eyeRenderViewport[1].Size = eyeRenderViewport[0].Size;

    _leftRendererViewport = math::ivec4(
        eyeRenderViewport[0].Pos.x,
        eyeRenderViewport[0].Pos.y,
        eyeRenderViewport[0].Size.w,
        eyeRenderViewport[0].Size.h
    );

    _rightRendererViewport = math::ivec4(
        eyeRenderViewport[1].Pos.x,
        eyeRenderViewport[1].Pos.y,
        eyeRenderViewport[1].Size.w,
        eyeRenderViewport[1].Size.h
    );

    _leftRenderer->viewport(_leftRendererViewport);
    _rightRenderer->viewport(_rightRendererViewport);

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

        _uvScaleOffset[eyeNum].first = math::vec2(ovrUVScaleOffset[0].x, ovrUVScaleOffset[0].y);
        _uvScaleOffset[eyeNum].second = math::vec2(ovrUVScaleOffset[1].x, ovrUVScaleOffset[1].y);
    }

    // FIXME: Direct to HMD mode.
    if (!!window)
        ovrHmd_AttachToWindow(_hmd, window, nullptr, nullptr);
}

void
NativeOculus::initializePostProcessingRenderer(std::shared_ptr<component::SceneManager> sceneManager)
{
    auto context = sceneManager->assets()->context();

    _renderTarget = render::Texture::create(context, _renderTargetWidth, _renderTargetHeight, false, true);
    _renderTarget->upload();
	_leftRenderer->renderTarget(_renderTarget);
	_rightRenderer->renderTarget(_renderTarget);

    auto geometries = createDistortionGeometry(context);
    auto loader = file::Loader::create(sceneManager->assets()->loader());

    loader->queue("effect/OculusVR/OculusVR.effect");

    auto complete = loader->complete()->connect([&](file::Loader::Ptr l)
    {
        auto effect = sceneManager->assets()->effect("effect/OculusVR/OculusVR.effect");

        auto materialLeftEye = material::Material::create();
        materialLeftEye->data()
			->set("eyeToSourceUVScale", _uvScaleOffset[0].first)
			->set("eyeToSourceUVOffset", _uvScaleOffset[0].second)
			->set("eyeRotationStart", math::mat4())
			->set("eyeRotationEnd", math::mat4())
			->set("texture", _renderTarget->sampler());

        auto materialRightEye = material::Material::create();
        materialRightEye->data()
			->set("eyeToSourceUVScale", _uvScaleOffset[1].first)
			->set("eyeToSourceUVOffset", _uvScaleOffset[1].second)
			->set("eyeRotationStart", math::mat4())
			->set("eyeRotationEnd", math::mat4())
			->set("texture", _renderTarget->sampler());

        _ppScene = scene::Node::create()
            ->addComponent(_ppRenderer)
            ->addComponent(Surface::create(geometries[0], materialLeftEye, effect))
            ->addComponent(Surface::create(geometries[1], materialRightEye, effect));
    });

    loader->load();
}

void
NativeOculus::targetRemoved()
{
    ovrHmd_Destroy(_hmd);
    ovr_Shutdown();
}

void
NativeOculus::updateViewport(int viewportWidth, int viewportHeight)
{
    // Renderer viewports and aspect ratio are updated into VRCamera, 
    // for Oculus we need a specific viewport given by SDK
    auto aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

    if (_renderTarget)
        aspectRatio = static_cast<float>(_renderTarget->width()) / static_cast<float>(_renderTarget->height());

    if (_leftRenderer)
    {
        if (_leftRendererViewport.z > 0 && _leftRendererViewport.w > 0)
            _leftRenderer->viewport(_leftRendererViewport);

        auto leftCamera = _leftRenderer->target()->component<PerspectiveCamera>();
        leftCamera->aspectRatio(aspectRatio);
        leftCamera->fieldOfView(getLeftEyeFov());
    }

    if (_rightRenderer)
    {
        if (_rightRendererViewport.z > 0 && _rightRendererViewport.w > 0)
            _rightRenderer->viewport(_rightRendererViewport);
     
        auto rightCamera = _rightRenderer->target()->component<PerspectiveCamera>();
        rightCamera->aspectRatio(aspectRatio);
        rightCamera->fieldOfView(getRightEyeFov());
    }

    _ppRenderer->viewport(math::ivec4(0, 0, viewportWidth, viewportHeight));
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
            static_cast<ovrEyeType>(eyeNum),
            _hmd->DefaultEyeFov[eyeNum],
            ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
            &meshData
        );

        auto vb = render::VertexBuffer::create(
            context,
            reinterpret_cast<float*>(meshData.pVertexData),
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

        vb->addAttribute("screenPosNDC", 2, 0);
        vb->addAttribute("timeWarpFactor", 1, 2);
        vb->addAttribute("vignetteFactor", 1, 3);
        vb->addAttribute("tanEyeAnglesR", 2, 4);
        vb->addAttribute("tanEyeAnglesG", 2, 6);
        vb->addAttribute("tanEyeAnglesB", 2, 8);
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

float
NativeOculus::getLeftEyeFov()
{
    return atan(_hmd->DefaultEyeFov[0].LeftTan + _hmd->DefaultEyeFov[0].RightTan);
}

float
NativeOculus::getRightEyeFov()
{
    return atan(_hmd->DefaultEyeFov[1].LeftTan + _hmd->DefaultEyeFov[1].RightTan);
}

void
NativeOculus::updateCameraOrientation(std::shared_ptr<scene::Node> target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    static ovrPosef eyeRenderPose[2];
    const static float BodyYaw(0.f);
    static OVR::Vector3f HeadPos(0.0f, 0.f, 0.f);
    static auto state = static_cast<OVR::CAPI::HMDState*>(_hmd->Handle);
    static auto renderInfo = state->RenderState.RenderInfo;

    HeadPos.y = ovrHmd_GetFloat(_hmd, OVR_KEY_EYE_HEIGHT, HeadPos.y);

    for (int eyeNum = 0; eyeNum < 2; eyeNum++)
    {
        ovrEyeType eye = _hmd->EyeRenderOrder[eyeNum];
        eyeRenderPose[eye] = ovrHmd_GetHmdPosePerEye(_hmd, eye);

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

        auto cameraNode = eyeNum == 0 ? leftCamera : rightCamera;

        auto viewMatrix = glm::make_mat4(reinterpret_cast<float*>(view.M));
        viewMatrix = math::inverse(viewMatrix);
        viewMatrix = math::transpose(viewMatrix);

        auto eyesLag = math::translate(math::vec3(viewAdjust.x, viewAdjust.y, viewAdjust.z));
        viewMatrix = eyesLag * viewMatrix;

        cameraNode->component<Transform>()->matrix(viewMatrix);

        // Update time warp matrices
        ovrMatrix4f twMatrices[2];
        ovrHmd_GetEyeTimewarpMatrices(_hmd, static_cast<ovrEyeType>(eyeNum), eyeRenderPose[eyeNum], twMatrices);

        auto rotationStart = glm::make_mat4(reinterpret_cast<float*>(twMatrices[0].M));
        auto rotationEnd = glm::make_mat4(reinterpret_cast<float*>(twMatrices[1].M));

        _ppScene->component<Surface>(eyeNum)->material()->data()
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
