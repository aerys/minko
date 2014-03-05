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
#include "minko/component/ParticleSystem.hpp"
#include "minko/particle/shape/EmitterShape.hpp"
#include "minko/particle/modifier/IParticleModifier.hpp"

using namespace minko;
using namespace minko::extension;
using namespace minko::file;

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
	msgpack::zone   mempool;
	msgpack::object deserialized;

    msgpack::unpacked msg;
    msgpack::unpack(&msg, serialized.data(), serialized.size() - 1);
    std::cout << msg.get() << std::endl;

    //msg.get()

    /*typedef msgpack::type:: MyType;

    MyType dst;
	msgpack::unpack(serialized.data(), serialized.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

    float   rate        = dst.a0;

    msgpack::unpack(serialized.data() + , serialized.size() - 1, NULL, &mempool, &deserialized);
	deserialized.convert(&dst);

    std::cout << "rate = " << rate << std::endl;*/

    return nullptr;
}

/*static*/
particle::shape::EmitterShape::Ptr
ParticlesExtension::deserializeEmitterShape(const SerializedShape& deserialized)
{
    return nullptr;
}

/*static*/
particle::modifier::IParticleModifier::Ptr
ParticlesExtension::deserializeParticleModifier(const SerializedModifier& deserialized)
{
    return nullptr;
}