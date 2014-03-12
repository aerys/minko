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

#include "minko/extension/PhysicsExtension.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/SceneParser.hpp"
#include "msgpack.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/component/bullet/SphereShape.hpp"
#include "minko/component/bullet/BoxShape.hpp"
#include "minko/component/bullet/ConeShape.hpp"
#include "minko/component/bullet/CylinderShape.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Any.hpp"
#include "minko/component/bullet/ColliderData.hpp"
#include "minko/component/bullet/Collider.hpp"

using namespace minko;
using namespace minko::extension;

void
PhysicsExtension::bind()
{
	file::SceneParser::registerComponent(50, std::bind(&PhysicsExtension::deserializePhysics,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));
}

std::shared_ptr<component::AbstractComponent>
PhysicsExtension::deserializePhysics(std::string&							serializedAnimation,
									 std::shared_ptr<file::AssetLibrary>	assetLibrary,
									 std::shared_ptr<file::Dependency>		dependencies)
{
	component::bullet::AbstractPhysicsShape::Ptr	deserializedShape;
	msgpack::zone									mempool;
	msgpack::object									deserialized;
	// shape type, shape data, delta transform, density, friction, restit, dynamic, trigger, filterGroup, filterMask
	msgpack::type::tuple<int, std::string, msgpack::type::tuple<uint, std::string>, float, float, float, bool, bool, uint, uint> dst;

	msgpack::unpack(serializedAnimation.data(), serializedAnimation.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

	std::vector<float> shapedata = deserialize::TypeDeserializer::deserializeVector<float>(dst.a1);

	uint shapeType = dst.a0;

	if (shapeType == 1) // Ball
		deserializedShape = component::bullet::SphereShape::create(shapedata[0]);
	else if (shapeType == 2) // Box
		deserializedShape = component::bullet::BoxShape::create(shapedata[0], shapedata[1], shapedata[2]);
	else if (shapeType == 3) // Cylinder
		deserializedShape = component::bullet::CylinderShape::create(shapedata[1], shapedata[0], shapedata[1]);
	else if (shapeType == 4) // Cone
		deserializedShape = component::bullet::ConeShape::create(shapedata[1], shapedata[0]);

	std::tuple<uint, std::string&> serializedMatrixTuple(dst.a2.a0, dst.a2.a1);

	auto deltaMatrix = Any::cast<math::Matrix4x4::Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

	if (!deltaMatrix->equals(math::Matrix4x4::create()))
		deserializedShape->initialize(deltaMatrix, math::Matrix4x4::create());

	auto mass = dst.a3 * deserializedShape->volume();

	if (dst.a6 == false)
		mass = 0.f;

	component::bullet::ColliderData::Ptr data = component::bullet::ColliderData::create(mass, deserializedShape);

	data->friction(dst.a4);
	data->restitution(dst.a5);
	data->triggerCollisions(dst.a7);
	data->disableDeactivation(true);

    const short filterGroup = short(dst.a8 & ((1<<16) - 1)); // overriden by node's layouts
    const short filterMask  = short(dst.a9 & ((1<<16) - 1)); 

    const short filterGroup = short(dst.a8 & ((1<<16) - 1)); // overriden by node's layouts
    const short filterMask  = short(dst.a9 & ((1<<16) - 1)); 

    data->collisionGroup(filterGroup);
    data->collisionMask(filterMask);

	return component::bullet::Collider::create(data);
}