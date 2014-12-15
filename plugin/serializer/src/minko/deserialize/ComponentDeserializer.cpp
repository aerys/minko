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
ComponentDeserializer::deserializeTransform(std::string&                        packed,
                                            std::shared_ptr<file::AssetLibrary> assetLibrary,
                                            std::shared_ptr<file::Dependency>   dependencies)
{
    msgpack::type::tuple<uint, std::string>     dst;

    minko::deserialize::unpack(dst, packed.data(), packed.size() - 1);

    std::tuple<uint, std::string&> serializedMatrixTuple(dst.get<0>(), dst.get<1>());

    Matrix4x4Ptr transformMatrix = Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

    return component::Transform::create(transformMatrix);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeProjectionCamera(std::string&                         packed,
                                                   std::shared_ptr<file::AssetLibrary>  assetLibrary,
                                                   std::shared_ptr<file::Dependency>    dependencies)
{
    std::string         dst;

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    return component::PerspectiveCamera::create(dstContent[0], dstContent[1], dstContent[2], dstContent[3]);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeAmbientLight(std::string&                         packed,
                                               std::shared_ptr<file::AssetLibrary>  assetLibrary,
                                               std::shared_ptr<file::Dependency>    dependencies)
{
    std::string                                 dst;
    std::shared_ptr<component::AmbientLight>    ambientLight = component::AmbientLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    ambientLight->ambient(dstContent[0]);
    ambientLight->color()->setTo(dstContent[1], dstContent[2], dstContent[3]);

    return ambientLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeDirectionalLight(std::string&                         packed,
                                                   std::shared_ptr<file::AssetLibrary>  assetLibrary,
                                                   std::shared_ptr<file::Dependency>    dependencies)
{
    std::string                                     dst;
    std::shared_ptr<component::DirectionalLight>    directionalLight = component::DirectionalLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    directionalLight->diffuse(dstContent[0]);
    directionalLight->specular(dstContent[1]);
    directionalLight->color()->setTo(dstContent[2], dstContent[3], dstContent[4]);

    return directionalLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializePointLight(std::string&                           packed,
                                             std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                             std::shared_ptr<file::Dependency>      dependencies)
{
    std::string                                 dst;
    std::shared_ptr<component::PointLight>      pointLight = component::PointLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    pointLight->diffuse(dstContent[0]);
    pointLight->specular(dstContent[1]);
    pointLight->attenuationCoefficients(dstContent[2], dstContent[3], dstContent[4]);
    pointLight->color()->setTo(dstContent[5], dstContent[6], dstContent[7]);

    return pointLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSpotLight(std::string&                        packed,
                                            std::shared_ptr<file::AssetLibrary> assetLibrary,
                                            std::shared_ptr<file::Dependency>   dependencies)
{
    std::string                                 dst;
    std::shared_ptr<component::SpotLight>       spotLight = component::SpotLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

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
ComponentDeserializer::deserializeSurface(std::string&                              packed,
                                          std::shared_ptr<file::AssetLibrary>       assetLibrary,
                                          std::shared_ptr<file::Dependency>         dependencies)
{
    msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string>   dst;
    msgpack::type::tuple<std::vector<SurfaceExtension>>                                 ext;

    unpack(dst, packed.data(), packed.size() - 1);

    geometry::Geometry::Ptr     geometry    = dependencies->getGeometryReference(dst.get<0>());
    material::Material::Ptr     material    = dependencies->getMaterialReference(dst.get<1>());
    render::Effect::Ptr         effect      = dependencies->getEffectReference(dst.get<2>());
    std::string                 technique   = "default";
    bool                        visible     = true;

    if (dst.get<3>().size() > 0)
    {
        unpack(ext, dst.get<3>().data(), dst.get<3>().size());

        for (int i = 0; i < ext.get<0>().size(); ++i)
        {
            auto extension = ext.get<0>()[i];

            if (extension.get<0>() == "visible")
            {
                visible = TypeDeserializer::deserializeVector<float>(extension.get<1>())[0] != 0.0;
            }
            else if (extension.get<0>() == "technique")
            {
                technique = extension.get<1>();
            }
        }
    }

    if (material == nullptr && dependencies->options()->material() != nullptr)
        material = dependencies->options()->material();

    if (effect == nullptr && dependencies->options()->effect() != nullptr)
        effect = dependencies->options()->effect();

    std::shared_ptr<component::Surface> surface = component::Surface::create(
        "",
        geometry,
        (material != nullptr ? material : assetLibrary->material("defaultMaterial")),
        (effect != nullptr ? effect : assetLibrary->effect("effect/Phong.effect")),
        technique
    );

    surface->visible(visible);

    return surface;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeRenderer(std::string&                         packed,
                                           std::shared_ptr<file::AssetLibrary>  assetLibrary,
                                           std::shared_ptr<file::Dependency>    dependencies)
{
    msgpack::type::tuple<unsigned int>      dst;
    std::shared_ptr<component::Renderer>    renderer = component::Renderer::create();

    unpack(dst, packed.data(), packed.size() - 1);

    renderer->backgroundColor(dst.get<0>());

    return renderer;
}


ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeAnimation(std::string&        packed,
                                            AssetLibraryPtr     assetLibrary,
                                            DependencyPtr       dependencies)
{
    std::vector<animation::AbstractTimeline::Ptr>                                    timelines;
    msgpack::type::tuple<uint, std::vector<uint>, VectorOfSerializedMatrix, bool>    dst;

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<math::Matrix4x4::Ptr>   matrices;
    std::vector<uint>                   timetable;
    uint                                duration    = dst.get<0>();
    bool                                interpolate = dst.get<3>();

    for (size_t i = 0; i < dst.get<1>().size(); ++i)
    {
        std::tuple<uint, std::string&> serializedMatrixTuple(dst.get<2>()[i].get<0>(), dst.get<2>()[i].get<1>());

        timetable.push_back(dst.get<1>()[i]);
        matrices.push_back(Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple)));
    }

    timelines.push_back(animation::Matrix4x4Timeline::create("transform.matrix", duration, timetable, matrices, interpolate));

    return component::Animation::create(timelines);
}

ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeSkinning(std::string&     packed,
                                           AssetLibraryPtr  assetLibrary,
                                           DependencyPtr    dependencies)
{
    typedef msgpack::type::tuple<std::string, std::string, std::string, SerializedMatrix>   boneType;
    msgpack::type::tuple<std::vector<boneType>, std::string, short>                         dst;

    unpack(dst, packed.data(), packed.size() - 1);

    auto duration        = dst.get<2>();
    auto skeletonName    = dst.get<1>();
    auto root            = dependencies->loadedRoot();

    std::vector<std::shared_ptr<geometry::Bone>>    bones;
    auto                                            numBones = dst.get<0>().size();

    std::vector<std::vector<uint>>                  bonesVertexIds;
    std::vector<std::vector<float>>                 bonesWeights;
    std::vector<scene::Node::Ptr>                   nodes;
    std::vector<scene::Node::Ptr>                   boneNodes;
    std::vector<std::shared_ptr<math::Matrix4x4>>   offsetMatrices;

    for (uint i = 0; i < numBones; i++)
    {
        auto                            serializedBone  = dst.get<0>()[i];
        std::tuple<uint, std::string&>  serializedMatrixTuple(serializedBone.get<3>().get<0>(), serializedBone.get<3>().get<1>());
        std::string                     nodeName        = serializedBone.get<0>();
        std::vector<uint>               vertexIntIds    = TypeDeserializer::deserializeVector<uint, uint>(serializedBone.get<1>());
        std::vector<unsigned short>     vertexShortIds(vertexIntIds.begin(), vertexIntIds.end());
        std::vector<float>              boneWeight      = TypeDeserializer::deserializeVector<float>(serializedBone.get<2>());
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
ComponentDeserializer::deserializeBoundingBox(std::string&                          packed,
                                              std::shared_ptr<file::AssetLibrary>   assetLibrary,
                                              std::shared_ptr<file::Dependency>     dependencies)
{
    std::string dst;

    unpack(dst, packed.data(), packed.size() - 1);

    auto componentData = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    auto component = component::BoundingBox::create(componentData[3],
                                                    componentData[4],
                                                    componentData[5],
                                                    math::Vector3::create(componentData[0],
                                                                          componentData[1],
                                                                          componentData[2]));

    return component;
}
