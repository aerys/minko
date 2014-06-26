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

#include "minko/extension/ParticlesExtension.hpp"
#include "minko/Types.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/SceneParser.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/component/ParticleSystem.hpp"
#include "minko/particle/sampler/Constant.hpp"
#include "minko/particle/sampler/RandomValue.hpp"
#include "minko/particle/sampler/LinearlyInterpolatedValue.hpp"
#include "minko/particle/shape/EmitterShape.hpp"
#include "minko/particle/shape/Box.hpp"
#include "minko/particle/shape/Cone.hpp"
#include "minko/particle/shape/Cylinder.hpp"
#include "minko/particle/shape/Point.hpp"
#include "minko/particle/shape/Sphere.hpp"
#include "minko/particle/modifier/IParticleModifier.hpp"
#include "minko/particle/modifier/StartAngularVelocity.hpp"
#include "minko/particle/modifier/StartColor.hpp"
#include "minko/particle/modifier/StartForce.hpp"
#include "minko/particle/modifier/StartRotation.hpp"
#include "minko/particle/modifier/StartSize.hpp"
#include "minko/particle/modifier/StartSprite.hpp"
#include "minko/particle/modifier/StartVelocity.hpp"
#include "minko/particle/modifier/ColorBySpeed.hpp"
#include "minko/particle/modifier/ColorOverTime.hpp"
#include "minko/particle/modifier/ForceOverTime.hpp"
#include "minko/particle/modifier/SizeOverTime.hpp"
#include "minko/particle/modifier/SizeBySpeed.hpp"
#include "minko/particle/modifier/VelocityOverTime.hpp"
#include "minko/data/ParticlesProvider.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::extension;
using namespace minko::file;
using namespace minko::particle;

void
ParticlesExtension::bind()
{
    SceneParser::registerComponent(serialize::ComponentId::PARTICLES, std::bind(
        &ParticlesExtension::deserializeParticles,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
}

/*static*/
component::AbstractComponent::Ptr
ParticlesExtension::deserializeParticles(std::string&       serialized, 
                                         AssetLibrary::Ptr  assets, 
                                         Dependency::Ptr    dependencies)
{
    typedef msgpack::type::tuple<unsigned short, float, uint, bool, bool, bool, uint, IdAndString, IdAndString, IdAndString, std::vector<IdAndString>>  SerializedParticles;

    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size() - 1, NULL, &mempool, &deserialized);

    SerializedParticles dst;
    deserialized.convert(&dst);

    const unsigned short    matId           = dst.a0;
    const float             rate            = dst.a1;
    const auto              startDirection  = particle::StartDirection(dst.a2);
    const bool              emit            = dst.a3;
    const bool              inWorldSpace    = dst.a4;
    const bool              zSorted         = dst.a5;
    const uint              countLimit      = dst.a6;
    auto                    lifetime        = deserializeFloatSampler(serialize::SamplerId(dst.a7.a0), dst.a7.a1);
    auto                    shape           = deserializeEmitterShape(serialize::EmitterShapeId(dst.a8.a0), dst.a8.a1);
    auto                    startVelocity   = deserializeFloatSampler(serialize::SamplerId(dst.a9.a0), dst.a9.a1);

    auto particles = component::ParticleSystem::create(
        assets,
        rate,
        lifetime,
        shape,
        startDirection,
        startVelocity
    );
    particles->emitting(emit);
	
    for (auto& modifier : dst.a10)
    {
        auto mod = deserializeParticleModifier(
            serialize::ModifierId(modifier.a0), 
            modifier.a1
        );
        
        if (mod)
             particles->add(mod);   
    }

    // retrieve the diffuse color and diffuse texture from associated material if any
    math::Vector4::Ptr              particleColor   = nullptr;
    render::AbstractTexture::Ptr    particleMap     = nullptr; // FIXME

    auto material = dependencies->getMaterialReference(matId);
    if (material)
    {
        // FIXME !!
        static const std::string PNAME_COLOR        = "diffuseColor";
        static const std::string PNAME_SPRITESHEET  = "diffuseMap";

        for (auto& pname : material->propertyNames())
        {
            auto pos            = pname.find_last_of('.');
            std::string suffix  = pos != std::string::npos ? pname.substr(pos + 1) : pname;

            if (suffix == PNAME_COLOR)
                particleColor = material->get<math::Vector4::Ptr>(pname, true);
            else if (suffix == PNAME_SPRITESHEET)
                particleMap = material->get<render::AbstractTexture::Ptr>(pname, true);
        }
    }

    if (particleColor)
        particles->material()->diffuseColor(particleColor);
    if (particleMap)
        particles->material()->diffuseMap(particleMap);

    return particles->play();
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeParticleModifier(serialize::ModifierId           id, 
                                                const std::string&              serialized)
{
    switch (id)
    {
    case minko::serialize::ModifierId::START_COLOR:
        return deserializeStartColorInitializer(serialized);
    case minko::serialize::ModifierId::START_FORCE:
        return deserializeStartForceInitializer(serialized);
    case minko::serialize::ModifierId::START_ROTATION:
        return deserializeStartRotationInitializer(serialized);
    case minko::serialize::ModifierId::START_SIZE:
        return deserializeStartSizeInitializer(serialized);
    case minko::serialize::ModifierId::START_SPRITE:
        return deserializeStartSpriteInitializer(serialized);
    case minko::serialize::ModifierId::START_VELOCITY:
        return deserializeStartVelocityInitializer(serialized);
    case minko::serialize::ModifierId::START_ANGULAR_VELOCITY:
        return deserializeStartAngularVelocityInitializer(serialized);
    case minko::serialize::ModifierId::COLOR_BY_SPEED:
        return deserializeColorBySpeedUpdater(serialized);
    case minko::serialize::ModifierId::COLOR_OVER_TIME:
        return deserializeColorOverTimeUpdater(serialized);
    case minko::serialize::ModifierId::FORCE_OVER_TIME:
        return deserializeForceOverTimeUpdater(serialized);
    case minko::serialize::ModifierId::SIZE_BY_SPEED:
        return deserializeSizeBySpeedUpdater(serialized);
    case minko::serialize::ModifierId::SIZE_OVER_TIME:
        return deserializeSizeOverTimeUpdater(serialized);
    case minko::serialize::ModifierId::VELOCITY_OVER_TIME:
        return deserializeVelocityOverTimeUpdater(serialized);
    default:
        throw std::logic_error("Failed to deserialized particle modifier.");
    }
    return nullptr;
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartColorInitializer(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto color = deserializeColorSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    return modifier::StartColor::create(color);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartForceInitializer(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString, IdAndString, IdAndString> dst;
    deserialized.convert(&dst);

    auto fx = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto fy = deserializeFloatSampler(serialize::SamplerId(dst.a1.a0), dst.a1.a1);
    auto fz = deserializeFloatSampler(serialize::SamplerId(dst.a2.a0), dst.a2.a1);
    return modifier::StartForce::create(fx, fy, fz);

}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartRotationInitializer(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto rotation = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    return modifier::StartRotation::create(rotation);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartSizeInitializer(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto size = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    return modifier::StartSize::create(size);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartSpriteInitializer(const std::string&            serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<uint, uint, IdAndString> dst;
    deserialized.convert(&dst);

    auto sprite = deserializeFloatSampler(serialize::SamplerId(dst.a2.a0), dst.a2.a1);
    return modifier::StartSprite::create(sprite, dst.a0, dst.a1);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartVelocityInitializer(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString, IdAndString, IdAndString> dst;
    deserialized.convert(&dst);

    auto vx = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto vy = deserializeFloatSampler(serialize::SamplerId(dst.a1.a0), dst.a1.a1);
    auto vz = deserializeFloatSampler(serialize::SamplerId(dst.a2.a0), dst.a2.a1);
    return modifier::StartVelocity::create(vx, vy, vz);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeStartAngularVelocityInitializer(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto angVelocity = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    return modifier::StartAngularVelocity::create(angVelocity);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeColorBySpeedUpdater(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto color          = deserializeColorSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto linearColor    = std::dynamic_pointer_cast<sampler::LinearlyInterpolatedValue<math::Vector3>>(color);
    if (linearColor == nullptr)
        throw std::logic_error("Failed to initialize color-by-speed modifier.");

    return modifier::ColorBySpeed::create(linearColor);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeColorOverTimeUpdater(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto color          = deserializeColorSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto linearColor    = std::dynamic_pointer_cast<sampler::LinearlyInterpolatedValue<math::Vector3>>(color);
    if (linearColor == nullptr)
        throw std::logic_error("Failed to initialize color-over-time modifier.");

    return modifier::ColorOverTime::create(linearColor);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeForceOverTimeUpdater(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString, IdAndString, IdAndString> dst;
    deserialized.convert(&dst);

    auto fx = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto fy = deserializeFloatSampler(serialize::SamplerId(dst.a1.a0), dst.a1.a1);
    auto fz = deserializeFloatSampler(serialize::SamplerId(dst.a2.a0), dst.a2.a1);
    return modifier::ForceOverTime::create(fx, fy, fz);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeSizeBySpeedUpdater(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto size           = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto linearSize     = std::dynamic_pointer_cast<sampler::LinearlyInterpolatedValue<float>>(size);
    if (linearSize == nullptr)
        throw std::logic_error("Failed to initialize size-by-speed modifier.");

    return modifier::SizeBySpeed::create(linearSize);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeSizeOverTimeUpdater(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString> dst;
    deserialized.convert(&dst);

    auto size           = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto linearSize     = std::dynamic_pointer_cast<sampler::LinearlyInterpolatedValue<float>>(size);
    if (linearSize == nullptr)
        throw std::logic_error("Failed to initialize size-over-time modifier.");

    return modifier::SizeOverTime::create(linearSize);
}

/*static*/
modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeVelocityOverTimeUpdater(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<IdAndString, IdAndString, IdAndString> dst;
    deserialized.convert(&dst);

    auto vx = deserializeFloatSampler(serialize::SamplerId(dst.a0.a0), dst.a0.a1);
    auto vy = deserializeFloatSampler(serialize::SamplerId(dst.a1.a0), dst.a1.a1);
    auto vz = deserializeFloatSampler(serialize::SamplerId(dst.a2.a0), dst.a2.a1);
    return modifier::VelocityOverTime::create(vx, vy, vz);
}

//---------------------------------------------------------------------
// EMITTER SHAPES
//---------------
/*static*/
shape::EmitterShape::Ptr
ParticlesExtension::deserializeEmitterShape(serialize::EmitterShapeId   id, 
                                            const std::string&          serialized)
{
    switch (id)
    {
    case minko::serialize::EmitterShapeId::CYLINDER:
        return deserializeCylinderShape(serialized);
    case minko::serialize::EmitterShapeId::CONE:
        return deserializeConeShape(serialized);
    case minko::serialize::EmitterShapeId::SPHERE:
        return deserializeSphereShape(serialized);
    case minko::serialize::EmitterShapeId::POINT:
        return deserializePointShape(serialized);
    case minko::serialize::EmitterShapeId::BOX:
        return deserializeBoxShape(serialized);
    case minko::serialize::EmitterShapeId::UNKNOWN:
    default:
        throw std::logic_error("Failed to deserialized emitter shape.");
    }
    return nullptr;
}

/*static*/
shape::EmitterShape::Ptr
ParticlesExtension::deserializeConeShape(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float, float> dst;
    deserialized.convert(&dst);

    return shape::Cone::create(dst.a0, dst.a1, dst.a2, dst.a3);
}

/*static*/
shape::EmitterShape::Ptr
ParticlesExtension::deserializeCylinderShape(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float> dst;
    deserialized.convert(&dst);

    return shape::Cylinder::create(dst.a0, dst.a1, dst.a2);
}

/*static*/
shape::EmitterShape::Ptr
ParticlesExtension::deserializePointShape(const std::string& serialized)
{
    return shape::Point::create();
}

/*static*/
shape::EmitterShape::Ptr
ParticlesExtension::deserializeSphereShape(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float> dst;
    deserialized.convert(&dst);

    return shape::Sphere::create(dst.a0, dst.a1);
}

/*static*/
shape::EmitterShape::Ptr
ParticlesExtension::deserializeBoxShape(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float, uint> dst;
    deserialized.convert(&dst);

    return shape::Box::create(dst.a0, dst.a1, dst.a2, dst.a3 != 0);
}


//---------------------------------------------------------------------
// SAMPLERS
//---------

/*static*/
sampler::Sampler<float>::Ptr
ParticlesExtension::deserializeFloatSampler(serialize::SamplerId   id, 
                                            const std::string&     serialized)
{
    switch (id)
    {
    case minko::serialize::SamplerId::CONSTANT_NUMBER:
        return deserializeConstantNumberSampler(serialized);
    case minko::serialize::SamplerId::LINEAR_NUMBER:
        return deserializeLinearNumberSampler(serialized);
    case minko::serialize::SamplerId::RANDOM_NUMBER:
        return deserializeRandomNumberSampler(serialized);
    case minko::serialize::SamplerId::UNKNOWN:
        return nullptr;
    default:
        throw std::logic_error("Failed to deserialized float sampler.");
    }
}

/*static*/
sampler::Sampler<math::Vector3>::Ptr
ParticlesExtension::deserializeColorSampler(serialize::SamplerId   id, 
                                             const std::string&     serialized)
{
    switch (id)
    {
    case minko::serialize::SamplerId::CONSTANT_COLOR:
        return deserializeConstantColorSampler(serialized);
    case minko::serialize::SamplerId::LINEAR_COLOR:
        return deserializeLinearColorSampler(serialized);
    case minko::serialize::SamplerId::RANDOM_COLOR:
        return deserializeRandomColorSampler(serialized);
    case minko::serialize::SamplerId::UNKNOWN:
        return nullptr;
    default:
        throw std::logic_error("Failed to deserialized color sampler.");
    }
}


/*static*/
sampler::Sampler<float>::Ptr
ParticlesExtension::deserializeConstantNumberSampler(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float> dst;
    deserialized.convert(&dst);

    return sampler::Constant<float>::create(dst.a0);
}

/*static*/
sampler::Sampler<float>::Ptr
ParticlesExtension::deserializeLinearNumberSampler(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float, float> dst;
    deserialized.convert(&dst);

    return sampler::LinearlyInterpolatedValue<float>::create(dst.a0, dst.a1, dst.a2, dst.a3);
}

/*static*/
sampler::Sampler<float>::Ptr
ParticlesExtension::deserializeRandomNumberSampler(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float> dst;
    deserialized.convert(&dst);

    return sampler::RandomValue<float>::create(dst.a0, dst.a1);
}

/*static*/
sampler::Sampler<math::Vector3>::Ptr
ParticlesExtension::deserializeConstantColorSampler(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float> dst;
    deserialized.convert(&dst);

    auto color = math::Vector3::create(dst.a0, dst.a1, dst.a2);
    return sampler::Constant<math::Vector3>::create(*color);
}

/*static*/
sampler::Sampler<math::Vector3>::Ptr
ParticlesExtension::deserializeLinearColorSampler(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float, float, float, float, float, float> dst;
    deserialized.convert(&dst);

    auto startColor = math::Vector3::create(dst.a0, dst.a1, dst.a2);
    auto endColor   = math::Vector3::create(dst.a3, dst.a4, dst.a5);
    return sampler::LinearlyInterpolatedValue<math::Vector3>::create(*startColor, *endColor, dst.a6, dst.a7);
}

/*static*/
sampler::Sampler<math::Vector3>::Ptr
ParticlesExtension::deserializeRandomColorSampler(const std::string& serialized)
{
    msgpack::zone   mempool;
    msgpack::object deserialized;
    msgpack::unpack(serialized.data(), serialized.size(), NULL, &mempool, &deserialized);

    msgpack::type::tuple<float, float, float, float, float, float> dst;
    deserialized.convert(&dst);

    auto minColor = math::Vector3::create(dst.a0, dst.a1, dst.a2);
    auto maxColor = math::Vector3::create(dst.a3, dst.a4, dst.a5);
    return sampler::RandomValue<math::Vector3>::create(*minColor, *maxColor);
}