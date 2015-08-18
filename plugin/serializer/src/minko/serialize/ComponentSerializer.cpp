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

#include "minko/serialize/ComponentSerializer.hpp"
#include "msgpack.hpp"
#include "minko/Types.hpp"
#include "minko/animation/AbstractTimeline.hpp"
#include "minko/animation/Matrix4x4Timeline.hpp"
#include "minko/component/Animation.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/Bone.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/Skin.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/Effect.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/Metadata.hpp"
#include "minko/math/Box.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/file/Dependency.hpp"

using namespace minko;
using namespace minko::serialize;


std::string
ComponentSerializer::serializeTransform(NodePtr			        node,
                                        AbstractComponentPtr    component,
										DependencyPtr	        dependencies)
{
	int8_t										type		= serialize::TRANSFORM;
	auto		                                transform	= std::static_pointer_cast<component::Transform>(component);
	std::stringstream							buffer;
	std::tuple<uint, std::string>				serialized	= serialize::TypeSerializer::serializeMatrix4x4(transform->matrix());
	msgpack::type::tuple<uint, std::string>		src(std::get<0>(serialized), std::get<1>(serialized));

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializePerspectiveCamera(NodePtr			        node,
                                                AbstractComponentPtr    component,
												DependencyPtr	        dependencies)
{
	int8_t              type = serialize::PROJECTION_CAMERA;
	auto                perspectiveCamera = std::static_pointer_cast<component::PerspectiveCamera>(component);
	std::stringstream   buffer;
	std::vector<float>  data;

	data.push_back(perspectiveCamera->aspectRatio());
	data.push_back(perspectiveCamera->fieldOfView());
	data.push_back(perspectiveCamera->zNear());
	data.push_back(perspectiveCamera->zFar());

	std::string src = serialize::TypeSerializer::serializeVector<float>(data);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}


std::string
ComponentSerializer::serializeAmbientLight(NodePtr			    node,
                                           AbstractComponentPtr component,
										   DependencyPtr	    dependencies)
{
	int8_t				type		= serialize::AMBIENT_LIGHT;
	auto	            ambient		= std::static_pointer_cast<component::AmbientLight>(component);
	std::stringstream	buffer;
	std::vector<float>	data;

	data.push_back(ambient->ambient());
	data.push_back(ambient->color().x);
	data.push_back(ambient->color().y);
	data.push_back(ambient->color().z);

	std::string src = serialize::TypeSerializer::serializeVector<float>(data);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeDirectionalLight(NodePtr			    node,
                                               AbstractComponentPtr component,
											   DependencyPtr	    dependencies)
{
	int8_t											type		= serialize::DIRECTIONAL_LIGHT;
	auto	directional	= std::static_pointer_cast<component::DirectionalLight>(component);
	std::stringstream								buffer;
	std::vector<float>								data;

	data.push_back(directional->diffuse());
	data.push_back(directional->specular());
	data.push_back(directional->color().x);
	data.push_back(directional->color().y);
	data.push_back(directional->color().z);

	std::string src = serialize::TypeSerializer::serializeVector<float>(data);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}


std::string
ComponentSerializer::serializePointLight(NodePtr		        node,
                                         AbstractComponentPtr   component,
										 DependencyPtr	        dependencies)
{
	int8_t				type	= serialize::POINT_LIGHT;
	auto	            point	= std::static_pointer_cast<component::PointLight>(component);
	std::stringstream	buffer;
	std::vector<float>	data;

	data.push_back(point->diffuse());
	data.push_back(point->specular());
	data.push_back(point->attenuationCoefficients().x);
	data.push_back(point->attenuationCoefficients().y);
	data.push_back(point->attenuationCoefficients().z);
	data.push_back(point->color().x);
	data.push_back(point->color().y);
	data.push_back(point->color().z);

	std::string src = serialize::TypeSerializer::serializeVector<float>(data);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeSpotLight(NodePtr			        node,
                                        AbstractComponentPtr    component,
										DependencyPtr	        dependencies)
{
	int8_t				type	= serialize::SPOT_LIGHT;
	auto	            spot	= std::static_pointer_cast<component::SpotLight>(component);
	std::stringstream	buffer;
	std::vector<float>	data;

	data.push_back(spot->diffuse());
	data.push_back(spot->specular());
	data.push_back(spot->attenuationCoefficients().x);
	data.push_back(spot->attenuationCoefficients().y);
	data.push_back(spot->attenuationCoefficients().z);
	data.push_back(spot->innerConeAngle());
	data.push_back(spot->outerConeAngle());
	data.push_back(spot->color().x);
	data.push_back(spot->color().y);
	data.push_back(spot->color().z);

	std::string src = serialize::TypeSerializer::serializeVector<float>(data);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeSurface(NodePtr		        node,
                                      AbstractComponentPtr  component,
									  DependencyPtr         dependencies)
{
	int8_t				type	= serialize::SURFACE;
	auto		        surface	= std::static_pointer_cast<component::Surface>(component);
	std::stringstream	buffer;

	uint materialId = dependencies->registerDependency(surface->material());
	uint geometryId = dependencies->registerDependency(surface->geometry());
	uint effectId	= dependencies->registerDependency(surface->effect());

	msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string> src(
		geometryId,
		materialId,
		effectId,
		getSurfaceExtension(node, surface)
    );

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::getSurfaceExtension(NodePtr node, SurfacePtr surface)
{
	std::vector<SimpleProperty> properties;

    properties.emplace_back("uuid", surface->uuid());

	std::string technique = surface->technique();

	if (surface->technique() != "default")
		properties.push_back(serializeSimpleProperty(std::string("technique"), technique));

	/*if (!surface->visible())
		properties.push_back(serializeSimpleProperty(std::string("visible"), surface->visible()));*/

	std::stringstream buffer;

	msgpack::pack(buffer, SimplePropertyVector(properties));

	return buffer.str();
}

std::string
ComponentSerializer::serializeRenderer(NodePtr			    node,
                                       AbstractComponentPtr component,
									   DependencyPtr	    dependencies)
{
	int8_t				type		= serialize::RENDERER;
	auto	            renderer	= std::static_pointer_cast<component::Renderer>(component);
	std::stringstream	buffer;

	msgpack::type::tuple<unsigned int> src(renderer->backgroundColor());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeMasterAnimation(NodePtr			        node,
                                              AbstractComponentPtr      component,
                                              DependencyPtr	            dependencies)
{
    const auto type = static_cast<int8_t>(serialize::MASTER_ANIMATION);
    auto animation = std::dynamic_pointer_cast<component::MasterAnimation>(component);

    std::stringstream buffer;

    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializeAnimation(NodePtr			        node,
                                        AbstractComponentPtr    component,
                                        DependencyPtr	        dependencies)
{
    auto type = static_cast<int8_t>(serialize::ANIMATION);
    auto animation = std::dynamic_pointer_cast<component::Animation>(component);
    std::stringstream buffer;

    auto src = msgpack::type::tuple<
        unsigned int,
        std::vector<unsigned int>,
        std::vector<msgpack::type::tuple<unsigned int, std::string>>,
        bool
    >();

    for (auto i = 0u; i < animation->numTimelines(); ++i)
    {
        auto timeline = std::static_pointer_cast<animation::Matrix4x4Timeline>(animation->timeline(i));

        src.get<0>() = timeline->duration();

        for (const auto& timeToMatrixPair : timeline->matrices())
        {
            auto serializedMatrix = TypeSerializer::serializeMatrix4x4(timeToMatrixPair.second);

            src.get<1>().push_back(timeToMatrixPair.first);
            src.get<2>().emplace_back(
                std::get<0>(serializedMatrix),
                std::get<1>(serializedMatrix)
            );
        }

        src.get<3>() = timeline->interpolate();
    }

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializeSkinning(NodePtr			    node,
                                       AbstractComponentPtr component,
                                       DependencyPtr	    dependencies)
{
    auto type = static_cast<int8_t>(serialize::SKINNING);
    auto skinning = std::dynamic_pointer_cast<component::Skinning>(component);
    std::stringstream buffer;

    auto skin = skinning->skin();

    auto src = msgpack::type::tuple<
        std::vector<msgpack::type::tuple<
            std::string, std::string, std::string, std::vector<msgpack::type::tuple<unsigned int, std::string>>
        >>,
        std::string,
        short
    >();

    src.get<1>() = node->name();
    src.get<2>() = skin->duration();

    for (auto i = 0u; i < skin->numBones(); ++i)
    {
        auto bone = skin->bone(i);

        auto serializedBoneMatrices = std::vector<msgpack::type::tuple<unsigned int, std::string>>(
            skin->numFrames()
        );

        for (auto frameId = 0u; frameId < skinning->skin()->numFrames(); ++frameId)
        {
            const auto& matrix = skin->matrices(frameId).at(i);

            const auto serializedMatrix = TypeSerializer::serializeMatrix4x4(matrix);

            serializedBoneMatrices[frameId] = msgpack::type::tuple<unsigned int, std::string>(
                std::get<0>(serializedMatrix),
                std::get<1>(serializedMatrix)
            );
        }

        auto serializedOffsetMatrix = TypeSerializer::serializeMatrix4x4(bone->offsetMatrix());

        auto serializedBone = msgpack::type::tuple<
            std::string,
            std::string,
            std::string,
            std::vector<msgpack::type::tuple<unsigned int, std::string>>
        >(
            bone->node()->name(),
            TypeSerializer::serializeVector<unsigned int>(std::vector<unsigned int>(
                bone->vertexIds().begin(),
                bone->vertexIds().end()
            )),
            TypeSerializer::serializeVector<float>(bone->vertexWeights()),
            serializedBoneMatrices
        );

        src.get<0>().push_back(serializedBone);
    }

    msgpack::pack(buffer, src);
    msgpack::pack(buffer, type);

    return buffer.str();
}

std::string
ComponentSerializer::serializeBoundingBox(NodePtr 			    node,
                                          AbstractComponentPtr  component,
					 				      DependencyPtr 	    dependencies)
{
    auto                boundingBox = std::static_pointer_cast<component::BoundingBox>(component);
	math::Box::Ptr 		box 		= boundingBox->modelSpaceBox();
	const math::vec3&	topRight 	= box->topRight();
    const math::vec3& 	bottomLeft  = box->bottomLeft();

	int8_t 				type 		= serialize::BOUNDINGBOX;
	std::stringstream	buffer;
	std::vector<float>	data;

	float centerX = (topRight.x + bottomLeft.x) / 2.0f;
	float centerY = (topRight.y + bottomLeft.y) / 2.0f;
	float centerZ = (topRight.z + bottomLeft.z) / 2.0f;

	data.push_back(centerX);
	data.push_back(centerY);
	data.push_back(centerZ);
	data.push_back(box->width());
	data.push_back(box->height());
	data.push_back(box->depth());

	std::string src = serialize::TypeSerializer::serializeVector<float>(data);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeMetadata(NodePtr              node,
                                       AbstractComponentPtr component,
                                       DependencyPtr 	    dependencies)
{
    auto metadata = std::dynamic_pointer_cast<component::Metadata>(component);

    std::stringstream buffer;
    msgpack::pack(buffer, metadata->data());
    msgpack::pack(buffer, (int8_t)serialize::METADATA);

    return buffer.str();
}
