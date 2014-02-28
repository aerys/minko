/*
Copyright (c) 2013 Aerys

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
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Animation.hpp"
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

using namespace minko;
using namespace minko::deserialize;

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeTransform(std::string&						serializedTransformData,
											std::shared_ptr<file::AssetLibrary>	assetLibrary,
											std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::zone									mempool;
	msgpack::object									deserialized;
	msgpack::type::tuple<uint, std::string>	dst;

	msgpack::unpack(serializedTransformData.data(), serializedTransformData.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	std::tuple<uint, std::string&> serializedMatrixTuple(dst.a0, dst.a1);

	Matrix4x4Ptr transformMatrix = Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));
	
	return component::Transform::create(transformMatrix);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeProjectionCamera(std::string&							serializedCameraData,
												   std::shared_ptr<file::AssetLibrary>	assetLibrary,
												   std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::zone										mempool;
	msgpack::object										deserialized;
	msgpack::type::tuple<float, float, float, float>	dst;

	msgpack::unpack(serializedCameraData.data(), serializedCameraData.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	return component::PerspectiveCamera::create(dst.a0, dst.a1, dst.a2, dst.a3);
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeAmbientLight(std::string&							serializedAmbientLight,
											   std::shared_ptr<file::AssetLibrary>	assetLibrary,
											   std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::zone										mempool;
	msgpack::object										deserialized;
	msgpack::type::tuple<float, float, float, float>	dst;
	std::shared_ptr<component::AmbientLight>			ambientLight = component::AmbientLight::create();
	
	msgpack::unpack(serializedAmbientLight.data(), serializedAmbientLight.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	ambientLight->ambient(dst.a0);
	ambientLight->color()->setTo(dst.a1, dst.a2, dst.a3);

	return ambientLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeDirectionalLight(std::string&							serializedDirectionalLight,
												   std::shared_ptr<file::AssetLibrary>	assetLibrary,
												   std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::zone											mempool;
	msgpack::object											deserialized;
	msgpack::type::tuple<float, float, float, float, float>	dst;
	std::shared_ptr<component::DirectionalLight>			directionalLight = component::DirectionalLight::create();
	
	msgpack::unpack(serializedDirectionalLight.data(), serializedDirectionalLight.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);
	
	directionalLight->diffuse(dst.a0);
	directionalLight->specular(dst.a1);
	directionalLight->color()->setTo(dst.a2, dst.a3, dst.a4);

	return directionalLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializePointLight(std::string&							serializedPointLight,
											 std::shared_ptr<file::AssetLibrary>	assetLibrary,
											 std::shared_ptr<file::Dependency>		dependencies)
{
	msgpack::zone													mempool;
	msgpack::object													deserialized;
	msgpack::type::tuple<float, float, float, float, float, float>	dst;
	std::shared_ptr<component::PointLight>							pointLight = component::PointLight::create();
	
	msgpack::unpack(serializedPointLight.data(), serializedPointLight.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	pointLight->diffuse(dst.a0);
	pointLight->specular(dst.a1);
	pointLight->attenuationCoefficients(dst.a2, 0.0f, 0.0f);
	pointLight->color()->setTo(dst.a3, dst.a4, dst.a5);

	return pointLight;
}
		
std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSpotLight(std::string&						serializedSpotLight,
										    std::shared_ptr<file::AssetLibrary>	assetLibrary,
											std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::zone																	mempool;
	msgpack::object																	deserialized;
	msgpack::type::tuple<float, float, float, float, float, float, float, float>	dst;
	std::shared_ptr<component::SpotLight>											spotLight = component::SpotLight::create();
	
	msgpack::unpack(serializedSpotLight.data(), serializedSpotLight.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	spotLight->diffuse(dst.a0);
	spotLight->specular(dst.a1);
	spotLight->attenuationCoefficients(dst.a2, 0.0f, 0.0f);
	spotLight->innerConeAngle(dst.a3);
	spotLight->outerConeAngle(dst.a4);
	spotLight->color()->setTo(dst.a5, dst.a6, dst.a7);

	return spotLight;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeSurface(std::string&							serializedSurface,
										  std::shared_ptr<file::AssetLibrary>	assetLibrary,
										  std::shared_ptr<file::Dependency>		dependencies)
{
	msgpack::zone																		mempool;
	msgpack::object																		deserialized;
	msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string>	dst;
	
	msgpack::unpack(serializedSurface.data(), serializedSurface.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	geometry::Geometry::Ptr		geometry	= dependencies->getGeometryReference(dst.a0);
	data::Provider::Ptr			material	= dependencies->getMaterialReference(dst.a1);
	render::Effect::Ptr			effect = dependencies->getEffectReference(dst.a2);

	if (material == nullptr && dependencies->options()->material() != nullptr)
		material = dependencies->options()->material();

	if (effect == nullptr && dependencies->options()->effect() != nullptr)
		effect = dependencies->options()->effect();

	std::shared_ptr<component::Surface>	surface = component::Surface::create(
		"",
		geometry,
		(material != nullptr ? material : assetLibrary->material("defaultMaterial")),
		(effect != nullptr ? effect : assetLibrary->effect("effect/Phong.effect")),
		dst.a3);

	return surface;
}

std::shared_ptr<component::AbstractComponent>
ComponentDeserializer::deserializeRenderer(std::string&							serializedRenderer,
										   std::shared_ptr<file::AssetLibrary>	assetLibrary,
										   std::shared_ptr<file::Dependency>	dependencies)
{
	msgpack::zone							mempool;
	msgpack::object							deserialized;
	msgpack::type::tuple<unsigned int>		dst;
	std::shared_ptr<component::Renderer>	renderer = component::Renderer::create();
	
	msgpack::unpack(serializedRenderer.data(), serializedRenderer.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	renderer->backgroundColor(dst.a0);

	return renderer;
}


ComponentDeserializer::AbsComponentPtr
ComponentDeserializer::deserializeAnimation(std::string&		serializedAnimation,
											AssetLibraryPtr		assetLibrary,
											DependencyPtr		dependencies)
{
	std::vector<animation::AbstractTimeline::Ptr>									timelines;
	msgpack::zone																	mempool;
	msgpack::object																	deserialized;
	msgpack::type::tuple<uint, std::vector<uint>, VectorOfSerializedMatrix, bool>	dst;

	msgpack::unpack(serializedAnimation.data(), serializedAnimation.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	std::vector<math::Matrix4x4::Ptr>	matrices;
	std::vector<uint>					timetable;
	uint								duration = dst.a0;
	bool								interpolate = dst.a3;

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
ComponentDeserializer::deserializeSkinning(std::string&		serializedAnimation,
										   AssetLibraryPtr	assetLibrary,
										   DependencyPtr	dependencies)
{
	msgpack::zone																	mempool;
	msgpack::object																	deserialized;
	msgpack::type::tuple<std::vector<msgpack::type::tuple<std::string, std::string, std::string, SerializedMatrix>>, std::string, short>	dst;

	msgpack::unpack(serializedAnimation.data(), serializedAnimation.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	auto duration		= dst.a2;
	auto skeletonName	= dst.a1;
	auto root			= dependencies->loadedRoot();

	std::vector<std::shared_ptr<geometry::Bone>> bones;
	auto numBones = dst.a0.size();

	std::vector<std::vector<uint>> bonesVertexIds;
	std::vector<std::vector<float>> bonesWeights;
	std::vector<scene::Node::Ptr> nodes;
	std::vector<std::shared_ptr<math::Matrix4x4>> offsetMatrices;

	for (uint i = 0; i < numBones; i++)
	{
		auto							serializedBone = dst.a0[i];
		std::tuple<uint, std::string&>	serializedMatrixTuple(serializedBone.a3.a0, serializedBone.a3.a1);
		std::string						nodeName		= serializedBone.a0;
		std::vector<uint>				vertexIntIds	= TypeDeserializer::deserializeVector<uint, uint>(serializedBone.a1);
		std::vector<unsigned short>		vertexShortIds(vertexIntIds.begin(), vertexIntIds.end());
		std::vector<float>				boneWeight		= TypeDeserializer::deserializeVector<float>(serializedBone.a2);
		auto							offsetMatrix	= Any::cast<Matrix4x4Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

		auto nodeSet = scene::NodeSet::create(root)->descendants(true, false)->where([&](scene::Node::Ptr n)
		{
			return n->name() == nodeName;
		});

		bones.push_back(geometry::Bone::create(nodeSet->nodes()[0], offsetMatrix, vertexShortIds, boneWeight));

	}

	// @fixme Pierre

	return component::AmbientLight::create();
}