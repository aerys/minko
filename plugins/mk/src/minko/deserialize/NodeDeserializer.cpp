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

#include "NodeDeserializer.hpp"

#include "minko/scene/Node.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/data/Provider.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Qark.hpp"
#include "minko/deserialize/GeometryDeserializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/MkOptions.hpp"

using namespace std;
using namespace minko;
using namespace minko::deserialize;

string&
NodeDeserializer::extractName(NodeInfo& nodeInfo)
{
	return Any::cast<string&>(nodeInfo["name"]);
}

shared_ptr<scene::Node>
NodeDeserializer::deserializeGroup(NodeInfo&		nodeInfo,
								   OptionsPtr		options,
								   NodeMap&			nodeMap)
{
	shared_ptr<scene::Node>		group			= scene::Node::create(extractName(nodeInfo));
	shared_ptr<math::Matrix4x4>	transformMatrix = TypeDeserializer::matrix4x4(nodeInfo["transformation"]);

	group->addComponent(component::Transform::create());
	group->component<component::Transform>()->matrix()->copyFrom(transformMatrix);

	return group;
}

shared_ptr<scene::Node>
NodeDeserializer::deserializeMesh(NodeInfo&			nodeInfo,
								  OptionsPtr		options,
								  NodeMap&			nodeMap)
{
	shared_ptr<scene::Node>		mesh			= scene::Node::create(extractName(nodeInfo));
	shared_ptr<math::Matrix4x4>	transformMatrix = TypeDeserializer::matrix4x4(nodeInfo["transform"]);

	mesh->addComponent(component::Transform::create());
	mesh->component<component::Transform>()->matrix()->copyFrom(transformMatrix);
    
	Qark::ByteArray		geometryObject;
	int					copyId			= -1;
	string				geometryName	= "";
	bool				technique		= false;
	bool				iscopy			= false;

	if (nodeInfo.find("technique") != nodeInfo.end())
		technique = Any::cast<bool&>(nodeInfo["technique"]);

	if (nodeInfo.find("copyId") != nodeInfo.end())
	{
		iscopy = true;
		copyId = Any::cast<int&>(nodeInfo["copyId"]);
	}
	else
	{
		geometryObject	= Any::cast<Qark::ByteArray&>(nodeInfo["geometry"]);
		copyId			= Any::cast<int&>(nodeInfo["geometryId"]);
	}


	try
	{
		if (nodeInfo.find("geometryName") != nodeInfo.end())
			geometryName = Any::cast<string&>(nodeInfo["geometryName"]);
	}
	catch (...)
	{
		geometryName = ("noName" + std::to_string(rand()));
	}

	vector<Any>&	bindingsId	= Any::cast<vector<Any>&>(nodeInfo["bindingsIds"]);
	int				materialId	= Any::cast<int&>(bindingsId[0]);
	bool computeTangent			= options->deserializedAssets()->material(materialId)->hasProperty("material.normalMap");

	GeometryDeserializer::deserializeGeometry(
		iscopy,
		geometryName,
		copyId,
		geometryObject,
		options->assetLibrary(),
		mesh,
		options->parseOptions(),
		computeTangent,
		options->deserializedAssets()->material(materialId),
		options->parseOptions()->effect()
	);

	return mesh;
}

shared_ptr<scene::Node>
NodeDeserializer::deserializeLight(NodeInfo&		nodeInfo,
								   OptionsPtr		options,
								   NodeMap&			nodeMap)
{
	shared_ptr<scene::Node> light = scene::Node::create(extractName(nodeInfo));
	shared_ptr<math::Matrix4x4>	transformMatrix = TypeDeserializer::matrix4x4(nodeInfo["transform"]);

	light->addComponent(component::Transform::create(transformMatrix));

	auto lightType = Any::cast<int>(nodeInfo["subType"]);
	auto color = Any::cast<uint>(nodeInfo["color"]);
	auto colorVec = math::Vector3::create(
		((color >> 24) & 0xff) / 255.f,
		((color >> 16) & 0xff) / 255.f,
		((color >> 8 )& 0xff) / 255.f
	);

	component::AbstractLight::Ptr lightComp = nullptr;

	if (lightType == 10) // directional light
		lightComp = component::DirectionalLight::create();
	else if (lightType == 11) // point light
	{
		auto pointLightComp = component::PointLight::create();

		try
		{
			pointLightComp->attenuationDistance(static_cast<float>(Any::cast<int>(nodeInfo["attenuationDistance"])));
		}
		catch (...)
		{
			pointLightComp->attenuationDistance(Any::cast<float>(nodeInfo["attenuationDistance"]));
		}

		lightComp = pointLightComp;
	}
	else if (lightType == 12) // spot light
	{
		auto spotLightComp = component::SpotLight::create();

		spotLightComp->attenuationDistance(Any::cast<float>(nodeInfo["ambient"]));
		spotLightComp->innerConeAngle(Any::cast<float>(nodeInfo["innerRadius"]));
		spotLightComp->outerConeAngle(Any::cast<float>(nodeInfo["outerRadius"]));

		lightComp = spotLightComp;
	}
	else if (lightType == 13) // ambient light
	{
		auto ambientComp = component::AmbientLight::create();

		ambientComp->ambient(Any::cast<float>(nodeInfo["ambient"]));
		lightComp = ambientComp;
	}
	
	if (lightComp)
	{
		auto discreteLightComp = std::dynamic_pointer_cast<component::AbstractDiscreteLight>(lightComp);

		lightComp->color(colorVec);
		if (discreteLightComp)
		{
			try
			{
				discreteLightComp->diffuse(Any::cast<float>(nodeInfo["diffuse"]));
			}
			catch (...)
			{
				discreteLightComp->diffuse(static_cast<float>(Any::cast<int>(nodeInfo["diffuse"])));
			}

			try
			{
				discreteLightComp->specular(Any::cast<float>(nodeInfo["specular"]));
			}
			catch (...)
			{
				discreteLightComp->specular(static_cast<float>(Any::cast<int>(nodeInfo["specular"])));
			}
		}

		light->addComponent(lightComp);
	}

	return light;
}

shared_ptr<scene::Node>
NodeDeserializer::deserializeCamera(NodeInfo&		nodeInfo,
									OptionsPtr	    options,
									NodeMap&		nodeMap)
{
	shared_ptr<scene::Node>		camera			= scene::Node::create(extractName(nodeInfo));
	shared_ptr<math::Matrix4x4>	transformMatrix = TypeDeserializer::matrix4x4(nodeInfo["transform"]);

	camera->addComponent(component::Transform::create());
	camera->component<component::Transform>()->matrix()->copyFrom(transformMatrix);
	camera->component<component::Transform>()->matrix()->prependRotationY(float(PI)); // otherwise the camera points the other way

	// extract camera information

	return camera;
}