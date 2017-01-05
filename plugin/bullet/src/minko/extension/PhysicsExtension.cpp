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
#include "minko/file/SceneWriter.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/component/bullet/SphereShape.hpp"
#include "minko/component/bullet/BoxShape.hpp"
#include "minko/component/bullet/ConeShape.hpp"
#include "minko/component/bullet/CylinderShape.hpp"
#include "minko/component/bullet/ConvexHullShape.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/Any.hpp"
#include "minko/component/bullet/ColliderData.hpp"
#include "minko/component/bullet/Collider.hpp"
#include "minko/Types.hpp"
#include "minko/file/GeometryWriter.hpp"

#include <btBulletDynamicsCommon.h>
#include "msgpack.hpp"

using namespace minko;
using namespace minko::deserialize;
using namespace minko::extension;
using namespace minko::file;


AbstractExtension::Ptr
PhysicsExtension::bind()
{
    file::SceneWriter::registerComponent(
            &typeid(component::bullet::Collider),
            std::bind(&PhysicsExtension::serializePhysics,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4)
        );

    file::SceneParser::registerComponent(
        serialize::ComponentId::COLLIDER, 
        std::bind(&PhysicsExtension::deserializePhysics, 
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    return shared_from_this();
}

std::shared_ptr<component::AbstractComponent>
PhysicsExtension::deserializePhysics(file::SceneVersion                     sceneVersion,
                                     std::string&                           serializedCollider,
                                     std::shared_ptr<file::AssetLibrary>    assetLibrary,
                                     std::shared_ptr<file::Dependency>      dependencies)
{
    component::bullet::AbstractPhysicsShape::Ptr deserializedShape;
    msgpack::zone mempool;
    msgpack::object deserialized;

    // shape type, shape data, delta transform, <density, friction, restit>, dynamic, trigger, filterGroup, filterMask, convexhull geometry points
    msgpack::type::tuple<int, std::string, msgpack::type::tuple<uint, std::string>, std::string, bool, bool, uint, uint, std::string> dst;
    

    //unpack(serializedCollider.data(), serializedCollider.size() - 1, nullptr, &mempool, &deserialized);
	unpack(dst, serializedCollider.data(), serializedCollider.size() - 1);

//    deserialized.convert(&dst);
    
    uint shapeType = dst.get<0>();
    std::vector<float> shapeData;
    const char* serializedConvexHull;

    if (shapeType != 5)
        shapeData = deserialize::TypeDeserializer::deserializeVector<float>(dst.get<1>());
    else
        serializedConvexHull = dst.get<1>().c_str();

    std::vector<float> physicsData = deserialize::TypeDeserializer::deserializeVector<float>(dst.get<3>());

	// TODO: Replace constant integer by Enum
    if (shapeType == 1) // Ball
    {
        deserializedShape = component::bullet::SphereShape::create(
            shapeData[0]
        );
    }
    else if (shapeType == 2) // Box
    {
        deserializedShape = component::bullet::BoxShape::create(
            shapeData[0],
            shapeData[1],
            shapeData[2]
        );
    }
    else if (shapeType == 3) // Cylinder
    {
        deserializedShape = component::bullet::CylinderShape::create(
            shapeData[1],
            0.5f * shapeData[0],
            shapeData[1]
        );
    }
    else if (shapeType == 4) // Cone
    {
        deserializedShape = component::bullet::ConeShape::create(
            shapeData[1],
            shapeData[0]
        );
    }
    else if (shapeType == 5) // ConvexHull
    {
        std::string serializedPointsString = dst.get<8>();
        
        const float* data = reinterpret_cast<const float*>(serializedPointsString.data());

        int numPoints = serializedPointsString.length() / (3 * sizeof(float));

        auto btShape = std::shared_ptr<btConvexHullShape>(new btConvexHullShape());
        for (uint i = 0; i < numPoints; i++)
        {
            float x = data[i*3 + 0];
            float y = data[i*3 + 1];
            float z = data[i*3 + 2];

            btVector3* vtx = new btVector3(x, y ,z);
            btShape->addPoint(*vtx, true);
        }
        deserializedShape = component::bullet::ConvexHullShape::create(            
            btShape
        );
    }

    std::tuple<uint, std::string&> serializedMatrixTuple(dst.get<2>().get<0>(), dst.get<2>().get<1>());

    auto deltaMatrix = Any::cast<math::mat4>(deserialize::TypeDeserializer::deserializeMatrix4x4(serializedMatrixTuple));

    if (deltaMatrix != math::mat4())
        deserializedShape->initialize(deltaMatrix, math::mat4());

    const auto density = physicsData[0];
    const auto friction = physicsData[1];
    const auto restitution = physicsData[2];

    auto mass = density * deserializedShape->volume();

    if (dst.get<4>() == false)
        mass = 0.0f;

    const short filterGroup = short(dst.get<6>() & ((1<<16) - 1)); // overriden by node's layouts
    
    const auto filterMask = scene::Layout(dst.get<7>());

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

std::string
PhysicsExtension::serializePhysics(std::shared_ptr<scene::Node>                 node,
                                    std::shared_ptr<component::AbstractComponent>    component,
                                    std::shared_ptr<file::AssetLibrary>         assetLibrary,
                                    std::shared_ptr<file::Dependency>           dependency)
{
    std::shared_ptr<component::bullet::Collider> collider = std::dynamic_pointer_cast<component::bullet::Collider>(component);

    auto shape = collider->colliderData()->shape();
    
    int shapetype = shape->type();
    std::vector<float> shapeData /*= serialize::TypeSerializer::seralizeVector<float>(shape->)*/;
    shapeData.resize(3 * sizeof (float));
    std::vector<float> physicsData;
    physicsData.resize(3 * sizeof (float));
    physicsData[0] = collider->colliderData()->mass() / shape->volume();//density
    physicsData[1] = collider->colliderData()->friction();//friction
    physicsData[2] = collider->colliderData()->restitution();//restitution

    std::string serializedPointsString;//only for convex hull

    if (shapetype == 1)//Ball
    {
        shapeData[0] = std::static_pointer_cast<component::bullet::SphereShape>(shape)->radius();
    }

    if (shapetype == 2)//Box
    {
        shapeData[0] = std::static_pointer_cast<component::bullet::BoxShape>(shape)->halfExtentX();
        shapeData[1] = std::static_pointer_cast<component::bullet::BoxShape>(shape)->halfExtentY();
        shapeData[2] = std::static_pointer_cast<component::bullet::BoxShape>(shape)->halfExtentZ();
    }

    if (shapetype == 3)//Cylinder
    {
        shapeData[0] = std::static_pointer_cast<component::bullet::CylinderShape>(shape)->halfExtentY() * 2;
        shapeData[1] = std::static_pointer_cast<component::bullet::CylinderShape>(shape)->halfExtentX();
    }

    if (shapetype == 4)//Cone
    {
        shapeData[0] = std::static_pointer_cast<component::bullet::ConeShape>(shape)->height();
        shapeData[1] = std::static_pointer_cast<component::bullet::ConeShape>(shape)->radius();
    }

    if (shapetype == 5)//ConvexHull
    {
        auto convexHull = std::static_pointer_cast<component::bullet::ConvexHullShape>(shape);

        std::vector<char> points;

        if (convexHull->geometry())
        {
            auto geometry = convexHull->geometry();
            auto vertexBuffer = geometry->vertexBuffer("position"); 
            uint vertexBufferSize = vertexBuffer->numVertices();

            auto attr = vertexBuffer->attribute("position");
            auto offset = vertexBuffer->attribute("position").offset;

            points.resize(vertexBufferSize * 3 * sizeof (float));
            float* pointsData = reinterpret_cast<float*>(points.data());

            auto vertexSize = vertexBuffer->vertexSize();

            for (uint i = 0; i < vertexBufferSize; ++i)
            {
                float x = vertexBuffer->data()[i * vertexSize + offset];
                float y = vertexBuffer->data()[i * vertexSize + offset + 1];
                float z = vertexBuffer->data()[i * vertexSize + offset + 2];
        
                pointsData[i*3 + 0] = x;
                pointsData[i*3 + 1] = y;
                pointsData[i*3 + 2] = z;
            }
        }
        else if (convexHull->getBtShape())
        {
            auto shape = convexHull->getBtShape();
            const auto shapePoints = shape->getPoints();
            const auto shapeNumPoints = shape->getNumPoints();

            points.resize(shapeNumPoints * 3 * sizeof (float));
            float* pointsData = reinterpret_cast<float*>(points.data());

            for (auto i = 0; i < shape->getNumPoints(); ++i)
            {
                const auto shapePoint = shapePoints[i];

                pointsData[i*3 + 0] = shapePoint.x();
                pointsData[i*3 + 1] = shapePoint.y();
                pointsData[i*3 + 2] = shapePoint.z();
            }
        }

       serializedPointsString.assign(points.data(), points.size());
    }

    bool isdynamic = collider->colliderData()->mass() != 0.0f;

    std::tuple<uint, std::string> deltaTransform = serialize::TypeSerializer::serializeMatrix4x4(shape->deltaTransform());
    msgpack::type::tuple<uint, std::string> transform;
    transform.get<0>() = std::get<0>(deltaTransform);
    transform.get<1>() = std::get<1>(deltaTransform);

    std::stringstream sbuf;

    // shape type, shape data, delta transform, <density, friction, restit>, dynamic, trigger, filterGroup, filterMask, convexhull geometry points
    msgpack::type::tuple<int, std::string, msgpack::type::tuple<uint, std::string>, std::string, bool, bool, uint, uint, std::string> dst(
        shapetype,
        serialize::TypeSerializer::serializeVector<float>(shapeData),
        transform,
        serialize::TypeSerializer::serializeVector<float>(physicsData),
        isdynamic,
        true,
        0,
        collider->layoutMask(),
        serializedPointsString
        );
    
    int8_t type = serialize::COLLIDER;

    msgpack::pack(sbuf, dst);
    msgpack::pack(sbuf, type);

    return sbuf.str();
}
