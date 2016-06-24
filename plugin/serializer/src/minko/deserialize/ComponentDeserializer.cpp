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
#include "minko/file/AbstractSerializerParser.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/ImageBasedLight.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Animation.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/geometry/Bone.hpp"
#include "minko/geometry/Skin.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Metadata.hpp"
#include "msgpack.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/file/Options.hpp"
#include "minko/animation/Matrix4x4Timeline.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::deserialize;

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeTransform(file::SceneVersion sceneVersion,
                                            std::string&                         packed,
											std::shared_ptr<file::AssetLibrary>	assetLibrary,
											std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::type::tuple<uint, std::string>		dst;

    minko::deserialize::unpack(dst, packed.data(), packed.size() - 1);
    uint& _0 = dst.get<0>();
    std::string& _1 = dst.get<1>();
	std::tuple<uint, std::string&> serializedMatrixTuple(_0, _1);

	auto transformMatrix = Any::cast<math::mat4>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

    // For .scene file of version 0.2.x or less we need to transpose transform matrices
    if (sceneVersion.major <= 0 && sceneVersion.minor < 3)
    {
        transformMatrix = math::transpose(transformMatrix);
    }

    return component::Transform::create(transformMatrix);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeProjectionCamera(file::SceneVersion sceneVersion,
                                                   std::string&                         packed,
												   std::shared_ptr<file::AssetLibrary>	assetLibrary,
												   std::shared_ptr<file::Dependency>	dependencies)
{
	std::string				dst;

    unpack(dst, packed.data(), packed.size() - 1);

	std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

	return component::PerspectiveCamera::create(dstContent[0], dstContent[1], dstContent[2], dstContent[3]);
}

component::AbstractComponent::Ptr
ComponentDeserializer::deserializeImageBasedLight(file::SceneVersion        sceneVersion,
                                                  std::string&              serializedImageBasedLight,
                                                  file::AssetLibrary::Ptr   assetLibrary,
                                                  file::Dependency::Ptr     dependencies)
{
    auto deserializedImageBasedLight = msgpack::type::tuple<std::string, unsigned int, unsigned int>();
    auto imageBasedLight = component::ImageBasedLight::create();

    unpack(deserializedImageBasedLight, serializedImageBasedLight.data(), serializedImageBasedLight.size() - 1);

    auto properties = deserialize::TypeDeserializer::deserializeVector<float>(deserializedImageBasedLight.get<0>());

    imageBasedLight
        ->diffuse(properties.at(0))
        ->specular(properties.at(1))
        ->orientation(properties.at(2));

    const auto irradianceMapId = deserializedImageBasedLight.get<1>();
    const auto radianceMapId = deserializedImageBasedLight.get<2>();

    if (irradianceMapId != 0u)
    {
        auto irradianceMap = dependencies->getTextureReference(irradianceMapId).texture;

        if (irradianceMap)
            imageBasedLight->irradianceMap(irradianceMap->sampler());
    }

    if (radianceMapId != 0u)
    {
        auto radianceMap = dependencies->getTextureReference(radianceMapId).texture;

        if (radianceMap)
            imageBasedLight->radianceMap(radianceMap->sampler());
    }

    return imageBasedLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeAmbientLight(file::SceneVersion sceneVersion,
                                               std::string&                         packed,
											   std::shared_ptr<file::AssetLibrary>	assetLibrary,
											   std::shared_ptr<file::Dependency>	dependencies)
{
	std::string 								dst;
	std::shared_ptr<component::AmbientLight>	ambientLight = component::AmbientLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

	std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    ambientLight->ambient(dstContent[0]);
	ambientLight->color(math::vec3(dstContent[1], dstContent[2], dstContent[3]));

    return ambientLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeDirectionalLight(file::SceneVersion sceneVersion,
                                                   std::string&                         packed,
												   std::shared_ptr<file::AssetLibrary>	assetLibrary,
												   std::shared_ptr<file::Dependency>	dependencies)
{
	std::string										dst;
	std::shared_ptr<component::DirectionalLight>	directionalLight = component::DirectionalLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

	std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    directionalLight->diffuse(dstContent[0]);
    directionalLight->specular(dstContent[1]);
	directionalLight->color(math::vec3(dstContent[2], dstContent[3], dstContent[4]));

    return directionalLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializePointLight(file::SceneVersion sceneVersion,
                                             std::string&                           packed,
											 std::shared_ptr<file::AssetLibrary>	assetLibrary,
											 std::shared_ptr<file::Dependency>		dependencies)
{
	std::string									dst;
	std::shared_ptr<component::PointLight>		pointLight = component::PointLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

	std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    pointLight->diffuse(dstContent[0]);
    pointLight->specular(dstContent[1]);
    pointLight->attenuationCoefficients(dstContent[2], dstContent[3], dstContent[4]);
	pointLight->color(math::vec3(dstContent[5], dstContent[6], dstContent[7]));

    return pointLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSpotLight(file::SceneVersion sceneVersion,
                                            std::string&                        packed,
										    std::shared_ptr<file::AssetLibrary>	assetLibrary,
											std::shared_ptr<file::Dependency>	dependencies)
{
	std::string									dst;
	std::shared_ptr<component::SpotLight>		spotLight = component::SpotLight::create();

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<float> dstContent = deserialize::TypeDeserializer::deserializeVector<float>(dst);

    spotLight->diffuse(dstContent[0]);
    spotLight->specular(dstContent[1]);
    spotLight->attenuationCoefficients(dstContent[2], dstContent[3], dstContent[4]);
    spotLight->innerConeAngle(dstContent[5]);
    spotLight->outerConeAngle(dstContent[6]);
	spotLight->color(math::vec3(dstContent[7], dstContent[8], dstContent[9]));

    return spotLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSurface(file::SceneVersion sceneVersion,
                                          std::string&                              packed,
										  std::shared_ptr<file::AssetLibrary>	assetLibrary,
										  std::shared_ptr<file::Dependency>		dependencies)
{
	msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string> dst;
	msgpack::type::tuple<std::vector<SurfaceExtension>>	ext;

    unpack(dst, packed.data(), packed.size() - 1);

	geometry::Geometry::Ptr		geometry	= dependencies->getGeometryReference(dst.get<0>());
	material::Material::Ptr		material	= dependencies->getMaterialReference(dst.get<1>());

    const auto effectId = dst.get<2>();

    auto effect = effectId != 0u
        ? dependencies->getEffectReference(effectId)
        : nullptr;

    auto uuid = std::string();
	auto technique = std::string("default");
	auto visible = true;

    if (dst.get<3>().size() > 0)
    {
        unpack(ext, dst.get<3>().data(), dst.get<3>().size());

        for (int i = 0; i < ext.get<0>().size(); ++i)
        {
            auto extension = ext.get<0>()[i];

            if (extension.get<0>() == "uuid")
            {
                uuid = extension.get<1>();
            }
            else if (extension.get<0>() == "visible")
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

    auto surface = component::Surface::Ptr();

    material = (material != nullptr ? material : assetLibrary->material("defaultMaterial"));

    if (uuid.empty())
    {
        surface = component::Surface::create(
            "",
            geometry,
            material,
            effect,
            technique
        );
    }
    else
    {
        surface = component::Surface::create(
            uuid,
            "",
            geometry,
            material,
            effect,
            technique
        );
    }

    return surface;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeRenderer(file::SceneVersion sceneVersion,
                                           std::string&                         packed,
										   std::shared_ptr<file::AssetLibrary>	assetLibrary,
										   std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::type::tuple<unsigned int>		dst;
	std::shared_ptr<component::Renderer>	renderer = component::Renderer::create();

    unpack(dst, packed.data(), packed.size() - 1);

    renderer->backgroundColor(dst.get<0>());

    return renderer;
}

ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeMasterAnimation(file::SceneVersion    sceneVersion,
                                                  std::string&          packed,
											      AssetLibraryPtr	    assetLibrary,
											      DependencyPtr	        dependencies)
{
    auto masterAnimation = component::MasterAnimation::create();

    auto labels = std::vector<std::pair<std::string, unsigned int>>();

    unpack(labels, packed.data(), packed.size() - 1);

    for (const auto& label : labels)
        masterAnimation->addLabel(label.first, label.second);

    return masterAnimation;
}

ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeAnimation(file::SceneVersion sceneVersion,
                                            std::string&        packed,
											AssetLibraryPtr	assetLibrary,
											DependencyPtr	dependencies)
{
	std::vector<animation::AbstractTimeline::Ptr>									timelines;
	msgpack::type::tuple<uint, std::vector<uint>, VectorOfSerializedMatrix, bool>	dst;

    unpack(dst, packed.data(), packed.size() - 1);

	std::vector<math::mat4>				matrices;
	std::vector<uint>                   timetable;
    uint                                duration    = dst.get<0>();
    bool                                interpolate = dst.get<3>();

    for (size_t i = 0; i < dst.get<1>().size(); ++i)
    {
        std::tuple<uint, std::string&> serializedMatrixTuple(dst.get<2>()[i].get<0>(), dst.get<2>()[i].get<1>());

		timetable.push_back(dst.get<1>()[i]);

        auto matrix = Any::cast<math::mat4>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

        if (sceneVersion.major <= 0 && sceneVersion.minor < 3)
            matrix = math::transpose(matrix);

		matrices.push_back(matrix);
	}

    timelines.push_back(animation::Matrix4x4Timeline::create("matrix", duration, timetable, matrices, interpolate));

    return component::Animation::create(timelines);
}

ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeSkinning(file::SceneVersion sceneVersion,
                                           std::string&     packed,
										   AssetLibraryPtr	assetLibrary,
										   DependencyPtr	dependencies)
{
	msgpack::zone mempool;
	msgpack::object deserialized;
	msgpack::type::tuple<
        std::vector<msgpack::type::tuple<std::string, std::string, std::string, std::vector<SerializedMatrix>>>,
        std::string,
        short
    > dst;

    unpack(dst, packed.data(), packed.size(), 0u);

	const auto duration = dst.get<2>();
	const auto skeletonName = dst.get<1>();

	auto root = dependencies->loadedRoot();

	const auto numBones = dst.get<0>().size();
    const auto numFrames = !dst.get<0>().empty() ? dst.get<0>().front().get<3>().size() : 0u;

    auto options = assetLibrary->loader()->options();
    auto context = assetLibrary->context();

    auto skin = geometry::Skin::create(numBones, duration, numFrames);

	for (auto boneId = 0u; boneId < numBones; boneId++)
	{
		auto serializedBone = dst.get<0>().at(boneId);

        for (auto frameId = 0u; frameId < numFrames; ++frameId)
        {
            const auto serializedMatrix = std::tuple<uint, std::string&>(
                serializedBone.get<3>().at(frameId).get<0>(),
                serializedBone.get<3>().at(frameId).get<1>()
            );

            const auto matrix = Any::cast<math::mat4>(TypeDeserializer::deserializeMatrix4x4(serializedMatrix));

            skin->matrix(frameId, boneId, matrix);
        }

		const auto nodeName = serializedBone.get<0>();
		const auto vertexIntIds = TypeDeserializer::deserializeVector<uint, uint>(serializedBone.get<1>());
		const auto vertexShortIds = std::vector<unsigned short>(vertexIntIds.begin(), vertexIntIds.end());
		const auto boneWeight = TypeDeserializer::deserializeVector<float>(serializedBone.get<2>());

		auto nodeSet = scene::NodeSet::create(root)
			->descendants(true, false)
			->where([&](scene::Node::Ptr n){ return n->name() == nodeName; });

		if (!nodeSet->nodes().empty())
		{
            auto node = nodeSet->nodes().front();

            auto bone = geometry::Bone::create(node, math::mat4(), vertexShortIds, boneWeight);

            skin->bone(boneId, bone);
		}
	}

	auto skinning =  component::Skinning::create(
        skin->reorganizeByVertices(),
        options->skinningMethod(),
        context,
		root,
		true
    );

    return skinning;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeBoundingBox(file::SceneVersion                    sceneVersion,
                                              std::string&                          packed,
                                              std::shared_ptr<file::AssetLibrary>   assetLibrary,
                                              std::shared_ptr<file::Dependency>     dependencies)
{
    std::string dst;

    unpack(dst, packed.data(), packed.size() - 1);

    auto componentData = deserialize::TypeDeserializer::deserializeVector<float>(dst);

	return component::BoundingBox::create(
        componentData[3],
        componentData[4],
        componentData[5],
        math::vec3(componentData[0], componentData[1], componentData[2])
    );
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeMetadata(file::SceneVersion                    sceneVersion,
                                           std::string&                          packed,
                                           std::shared_ptr<file::AssetLibrary>   assetLibrary,
                                           std::shared_ptr<file::Dependency>     dependencies)
{
    component::Metadata::Data data;

    unpack(data, packed.data(), packed.size() - 1);

	return component::Metadata::create(data);
}
