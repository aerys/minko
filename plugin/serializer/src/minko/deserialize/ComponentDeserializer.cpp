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

#include "minko/deserialize/ComponentDeserializer.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Animation.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/component/Renderer.hpp"
#include "msgpack.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/file/Options.hpp"
#include "minko/animation/Matrix4x4Timeline.hpp"
#include "minko/geometry/Bone.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/scene/Node.hpp"

#include "SkinningComponentDeserializer.hpp"

using namespace minko;
using namespace minko::deserialize;

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeTransform(std::string&                        serializedTransformData,
                                            std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                            std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::zone                                mempool;
    msgpack::object                                deserialized;
    msgpack::type::tuple<uint, std::string>        dst;

    msgpack::unpack(serializedTransformData.data(), serializedTransformData.size() - 1, NULL, &mempool, &deserialized);
    deserialized.convert(&dst);

    std::tuple<uint, std::string&> serializedMatrixTuple(dst.a0, dst.a1);

    Matrix4x4Ptr transformMatrix = Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

    return component::Transform::create(transformMatrix);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeProjectionCamera(std::string&                            serializedCameraData,
                                                   std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                                   std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::zone            mempool;
    msgpack::object            deserialized;
    std::string                dst;

    msgpack::unpack(serializedCameraData.data(), serializedCameraData.size() - 1, NULL, &mempool, &deserialized);

    if (deserialized.type != msgpack::type::RAW)
        return component::PerspectiveCamera::create(800.f / 600.F);

    deserialized.convert(&dst);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    return component::PerspectiveCamera::create(dstContent[0], dstContent[1], dstContent[2], dstContent[3]);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeAmbientLight(std::string&                            serializedAmbientLight,
                                               std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                               std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::zone                                mempool;
    msgpack::object                                deserialized;
    std::string                                 dst;
    std::shared_ptr<component::AmbientLight>    ambientLight = component::AmbientLight::create();

    msgpack::unpack(serializedAmbientLight.data(), serializedAmbientLight.size() - 1, NULL, &mempool, &deserialized);

    if (deserialized.type != msgpack::type::RAW)
        return ambientLight;

    deserialized.convert(&dst);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    ambientLight->ambient(dstContent[0]);
    ambientLight->color()->setTo(dstContent[1], dstContent[2], dstContent[3]);

    return ambientLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeDirectionalLight(std::string&                            serializedDirectionalLight,
                                                   std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                                   std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::zone                                    mempool;
    msgpack::object                                    deserialized;
    std::string                                        dst;
    std::shared_ptr<component::DirectionalLight>    directionalLight = component::DirectionalLight::create();

    msgpack::unpack(serializedDirectionalLight.data(), serializedDirectionalLight.size() - 1, NULL, &mempool, &deserialized);

    if (deserialized.type != msgpack::type::RAW)
        return directionalLight;

    deserialized.convert(&dst);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    directionalLight->diffuse(dstContent[0]);
    directionalLight->specular(dstContent[1]);
    directionalLight->color()->setTo(dstContent[2], dstContent[3], dstContent[4]);

    return directionalLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializePointLight(std::string&                            serializedPointLight,
                                             std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                             std::shared_ptr<file::Dependency>        dependencies)
{
    msgpack::zone                                mempool;
    msgpack::object                                deserialized;
    std::string                                    dst;
    std::shared_ptr<component::PointLight>        pointLight = component::PointLight::create();

    msgpack::unpack(serializedPointLight.data(), serializedPointLight.size() - 1, NULL, &mempool, &deserialized);

    if (deserialized.type != msgpack::type::RAW)
        return pointLight;

    deserialized.convert(&dst);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    pointLight->diffuse(dstContent[0]);
    pointLight->specular(dstContent[1]);
    pointLight->attenuationCoefficients(dstContent[2], dstContent[3], dstContent[4]);
    pointLight->color()->setTo(dstContent[5], dstContent[6], dstContent[7]);

    return pointLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSpotLight(std::string&                        serializedSpotLight,
                                            std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                            std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::zone                                mempool;
    msgpack::object                                deserialized;
    std::string                                    dst;
    std::shared_ptr<component::SpotLight>        spotLight = component::SpotLight::create();

    msgpack::unpack(serializedSpotLight.data(), serializedSpotLight.size() - 1, NULL, &mempool, &deserialized);

    if (deserialized.type != msgpack::type::RAW)
        return spotLight;

    deserialized.convert(&dst);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    spotLight->diffuse(dstContent[0]);
    spotLight->specular(dstContent[1]);
    spotLight->attenuationCoefficients(dstContent[2], dstContent[3], dstContent[4]);
    spotLight->innerConeAngle(dstContent[5]);
    spotLight->outerConeAngle(dstContent[6]);
    spotLight->color()->setTo(dstContent[7], dstContent[8], dstContent[9]);

    return spotLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSurface(std::string&                            serializedSurface,
                                          std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                          std::shared_ptr<file::Dependency>        dependencies)
{
    msgpack::zone                                                                        mempool;
    msgpack::object                                                                        deserialized;
    msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string>    dst;
    msgpack::type::tuple<std::vector<SurfaceExtension>>                                    ext;

    msgpack::unpack(serializedSurface.data(), serializedSurface.size() - 1, NULL, &mempool, &deserialized);
    deserialized.convert(&dst);

    geometry::Geometry::Ptr        geometry    = dependencies->getGeometryReference(dst.a0);
	material::Material::Ptr		material	= dependencies->getMaterialReference(dst.a1);
    render::Effect::Ptr            effect        = dependencies->getEffectReference(dst.a2);
    std::string                    technique    = "default";
    bool                        visible        = true;

    if (dst.a3.size() > 0)
    {
        msgpack::unpack(dst.a3.data(), dst.a3.size(), NULL, &mempool, &deserialized);
        deserialized.convert(&ext);

        for (int i = 0; i < ext.a0.size(); ++i)
        {
            auto extension = ext.a0[i];


            if (extension.a0 == "visible")
            {
                visible = TypeDeserializer::deserializeVector<float>(extension.a1)[0] != 0.0;
            }
            else if (extension.a0 == "technique")
            {
                technique = extension.a1;
            }
        }
    }


    if (material == nullptr && dependencies->options()->material() != nullptr)
        material = dependencies->options()->material();

    if (effect == nullptr && dependencies->options()->effect() != nullptr)
        effect = dependencies->options()->effect();

    std::shared_ptr<component::Surface>    surface = component::Surface::create(
        "",
        geometry,
        (material != nullptr ? material : assetLibrary->material("defaultMaterial")),
        (effect != nullptr ? effect : assetLibrary->effect("effect/Phong.effect")),
        technique);

    surface->visible(visible);

    return surface;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeRenderer(std::string&                            serializedRenderer,
                                           std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                           std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::zone                            mempool;
    msgpack::object                            deserialized;
    msgpack::type::tuple<unsigned int>        dst;
    std::shared_ptr<component::Renderer>    renderer = component::Renderer::create();

    msgpack::unpack(serializedRenderer.data(), serializedRenderer.size() - 1, NULL, &mempool, &deserialized);
    deserialized.convert(&dst);

    renderer->backgroundColor(dst.a0);

    return renderer;
}


ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeAnimation(std::string&        serializedAnimation,
                                            AssetLibraryPtr        assetLibrary,
                                            DependencyPtr        dependencies)
{
    std::vector<animation::AbstractTimeline::Ptr>                                    timelines;
    msgpack::zone                                                                    mempool;
    msgpack::object                                                                    deserialized;
    msgpack::type::tuple<uint, std::vector<uint>, VectorOfSerializedMatrix, bool>    dst;

    msgpack::unpack(serializedAnimation.data(), serializedAnimation.size() - 1, NULL, &mempool, &deserialized);
    deserialized.convert(&dst);

    std::vector<math::Matrix4x4::Ptr>    matrices;
    std::vector<uint>                    timetable;
    uint                                duration = dst.a0;
    bool                                interpolate = dst.a3;

    for (size_t i = 0; i < dst.a1.size(); ++i)
    {
        std::tuple<uint, std::string&> serializedMatrixTuple(dst.a2[i].a0, dst.a2[i].a1);

        timetable.push_back(dst.a1[i]);
        matrices.push_back(Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple)));
    }

    timelines.push_back(animation::Matrix4x4Timeline::create("transform.matrix", duration, timetable, matrices, interpolate));

    return component::Animation::create(timelines);
}

ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeSkinning(std::string&        serializedAnimation,
                                           AssetLibraryPtr    assetLibrary,
                                           DependencyPtr    dependencies)
{
    msgpack::zone                                                                    mempool;
    msgpack::object                                                                    deserialized;
    msgpack::type::tuple<std::vector<msgpack::type::tuple<std::string, std::string, std::string, SerializedMatrix>>, std::string, short>    dst;

    msgpack::unpack(serializedAnimation.data(), serializedAnimation.size() - 1, NULL, &mempool, &deserialized);
    deserialized.convert(&dst);

    auto duration        = dst.a2;
    auto skeletonName    = dst.a1;
    auto root            = dependencies->loadedRoot();

    std::vector<std::shared_ptr<geometry::Bone>> bones;
    auto numBones = dst.a0.size();

    std::vector<std::vector<uint>> bonesVertexIds;
    std::vector<std::vector<float>> bonesWeights;
    std::vector<scene::Node::Ptr> nodes;
	std::vector<scene::Node::Ptr> boneNodes;
    std::vector<std::shared_ptr<math::Matrix4x4>> offsetMatrices;

    for (uint i = 0; i < numBones; i++)
    {
        auto                            serializedBone = dst.a0[i];
        std::tuple<uint, std::string&>    serializedMatrixTuple(serializedBone.a3.a0, serializedBone.a3.a1);
        std::string                        nodeName        = serializedBone.a0;
        std::vector<uint>                vertexIntIds    = TypeDeserializer::deserializeVector<uint, uint>(serializedBone.a1);
        std::vector<unsigned short>        vertexShortIds(vertexIntIds.begin(), vertexIntIds.end());
        std::vector<float>                boneWeight        = TypeDeserializer::deserializeVector<float>(serializedBone.a2);
        auto                            offsetMatrix    = Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

        auto nodeSet = scene::NodeSet::create(root)
            ->descendants(true, false)
            ->where([&](scene::Node::Ptr n){ return n->name() == nodeName; });

        if (!nodeSet->nodes().empty())
		{

			bones.push_back(geometry::Bone::create(offsetMatrix, vertexShortIds, boneWeight));
			boneNodes.push_back(nodeSet->nodes()[0]);
		}
    }

    return SkinningComponentDeserializer::computeSkinning(
        assetLibrary->loader()->options(),
        assetLibrary->context(),
        bones,
		boneNodes,
        root->children().size() == 1 ? root->children().front() : root  // FIXME (for soccerpunch) there is one extra level wrt minko studio ! ->issues w/ precomputation and collider
   );
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeBoundingBox(std::string&                            serializedBoundingBox,
                                              std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                              std::shared_ptr<file::Dependency>     dependencies)
{
    msgpack::zone mempool;
    msgpack::object deserialized;
    std::string dst;

    msgpack::unpack(serializedBoundingBox.data(), serializedBoundingBox.size() - 1, NULL, &mempool, &deserialized);

    if (deserialized.type != msgpack::type::RAW)
        return component::BoundingBox::create();

    deserialized.convert(&dst);

    auto componentData = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    auto component = component::BoundingBox::create(componentData[3],
                                                    componentData[4],
                                                    componentData[5],
                                                    math::Vector3::create(componentData[0],
                                                                          componentData[1],
                                                                          componentData[2]));

    return component;
}
