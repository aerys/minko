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

	if (nodeInfo.find("geometryName") != nodeInfo.end())
		geometryName = Any::cast<string&>(nodeInfo["geometryName"]);

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

	// extract light information

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
	camera->component<component::Transform>()->matrix()->prependRotationY(PI); // otherwise the camera points the other way

	// extract camera information

	return camera;
}