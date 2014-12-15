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

#include "minko/extension/PhysicsExtension.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/SceneParser.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/component/bullet/SphereShape.hpp"
#include "minko/component/bullet/BoxShape.hpp"
#include "minko/component/bullet/ConeShape.hpp"
#include "minko/component/bullet/CylinderShape.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/deserialize/Unpacker.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Any.hpp"
#include "minko/component/bullet/ColliderData.hpp"
#include "minko/component/bullet/Collider.hpp"

using namespace minko;
using namespace minko::deserialize;
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
PhysicsExtension::deserializePhysics(std::string&                           packed,
                                     std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                     std::shared_ptr<file::Dependency>      dependencies)
{
    component::bullet::AbstractPhysicsShape::Ptr    deserializedShape;
    // shape type, shape data, delta transform, <density, friction, restit>, dynamic, trigger, filterGroup, filterMask
    msgpack::type::tuple<int, std::string, msgpack::type::tuple<uint, std::string>, std::string, bool, bool, uint, uint> dst;

    unpack(dst, packed.data(), packed.size() - 1);

    std::vector<float> shapeData = deserialize::TypeDeserializer::deserializeVector<float>(dst.get<1>());
    std::vector<float> physicsData = deserialize::TypeDeserializer::deserializeVector<float>(dst.get<3>());

    uint shapeType = dst.get<0>();

    if (shapeType == 1) // Ball
        deserializedShape = component::bullet::SphereShape::create(
            shapeData[0]
        );
    else if (shapeType == 2) // Box
        deserializedShape = component::bullet::BoxShape::create(
            shapeData[0],
            shapeData[1],
            shapeData[2]
        );
    else if (shapeType == 3) // Cylinder
        deserializedShape = component::bullet::CylinderShape::create(
            shapeData[1],
            0.5f * shapeData[0],
            shapeData[1]
        );
    else if (shapeType == 4) // Cone
        deserializedShape = component::bullet::ConeShape::create(
            shapeData[1],
            shapeData[0]
        );

    std::tuple<uint, std::string&> serializedMatrixTuple(dst.get<2>().get<0>(), dst.get<2>().get<1>());

    auto deltaMatrix = Any::cast<math::Matrix4x4::Ptr>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

    if (!deltaMatrix->equals(math::Matrix4x4::create()))
        deserializedShape->initialize(deltaMatrix, math::Matrix4x4::create());

    const auto    density        = physicsData[0];
    const auto    friction    = physicsData[1];
    const auto    restitution    = physicsData[2];

    auto mass = density * deserializedShape->volume();

    if (dst.get<4>() == false)
        mass = 0.0f;

    const short filterGroup = short(dst.get<6>() & ((1<<16) - 1)); // overriden by node's layouts
    const auto    filterMask = Layouts(dst.get<7>());

    auto data = component::bullet::ColliderData::create(
        mass,
        deserializedShape,
        restitution,
        friction
        );

    auto collider = component::bullet::Collider::create(data)
        //->collisionGroup(filterGroup) // information stored in node layouts
        ->triggerCollisions(dst.get<7>() != 0);

    collider->layoutMask(filterMask);

    return collider;
}
