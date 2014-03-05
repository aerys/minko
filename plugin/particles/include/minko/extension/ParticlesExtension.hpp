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

#pragma once

#include "msgpack.hpp"

#include "minko/Common.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/ParticlesCommon.hpp"
#include "minko/extension/AbstractExtension.hpp"
#include "minko/serialize/ParticlesTypes.hpp"

namespace minko
{
    namespace extension
    {
        class ParticlesExtension: public AbstractExtension
        {
        public:
            typedef std::shared_ptr<ParticlesExtension>                                             Ptr;

        private:
            typedef std::shared_ptr<component::AbstractComponent>                                   AbsComponentPtr;
            typedef std::shared_ptr<file::AssetLibrary>                                             AssetLibraryPtr;
            typedef std::shared_ptr<file::Dependency>                                               DependencyPtr;
            typedef std::shared_ptr<render::AbstractTexture>                                        AbsTexturePtr;

            typedef std::shared_ptr<particle::modifier::IParticleModifier>                          ParticleModifierPtr;
            typedef std::shared_ptr<particle::shape::EmitterShape>                                  EmitterShapePtr;
            typedef std::shared_ptr<particle::sampler::Sampler<float>>                              FloatSamplerPtr;
            typedef std::shared_ptr<particle::sampler::Sampler<math::Vector3>>                      ColorSamplerPtr;

            typedef msgpack::type::tuple<uint, std::string>                                         IdAndString;
       public:
            inline static
            Ptr
            initialize()
            {
                return std::shared_ptr<ParticlesExtension>(new ParticlesExtension());
            }

            void
            bind();

            static
            AbsComponentPtr
            deserializeParticles(std::string&, AssetLibraryPtr, DependencyPtr);

        private:
            inline
            ParticlesExtension()
            {
            }

            static
            EmitterShapePtr
            deserializeEmitterShape(serialize::EmitterShapeId, const std::string&);

            static 
            EmitterShapePtr
            deserializeConeShape(const std::string&);

            static 
            EmitterShapePtr
            deserializeCylinderShape(const std::string&);

            static 
            EmitterShapePtr
            deserializePointShape(const std::string&);

            static 
            EmitterShapePtr
            deserializeSphereShape(const std::string&);

            static 
            EmitterShapePtr
            deserializeBoxShape(const std::string&);

            static
            FloatSamplerPtr
            deserializeFloatSampler(serialize::SamplerId, const std::string&);

            static
            ColorSamplerPtr
            deserializeColorSampler(serialize::SamplerId, const std::string&);

            static
            FloatSamplerPtr
            deserializeConstantNumberSampler(const std::string&);

            static
            FloatSamplerPtr
            deserializeLinearNumberSampler(const std::string&);

            static
            FloatSamplerPtr
            deserializeRandomNumberSampler(const std::string&);

            static
            ColorSamplerPtr
            deserializeConstantColorSampler(const std::string&);

            static
            ColorSamplerPtr
            deserializeLinearColorSampler(const std::string&);

            static
            ColorSamplerPtr
            deserializeRandomColorSampler(const std::string&);

            static
            ParticleModifierPtr
            deserializeParticleModifier(serialize::ModifierId, const std::string&, AbsTexturePtr);

            static
            ParticleModifierPtr
            deserializeStartColorInitializer(const std::string&);

            static
            ParticleModifierPtr
            deserializeStartForceInitializer(const std::string&);

            static
            ParticleModifierPtr
            deserializeStartRotationInitializer(const std::string&);

            static
            ParticleModifierPtr
            deserializeStartSizeInitializer(const std::string&);

            static
            ParticleModifierPtr
            deserializeStartSpriteInitializer(const std::string&, AbsTexturePtr);

            static
            ParticleModifierPtr
            deserializeStartVelocityInitializer(const std::string&);

            static
            ParticleModifierPtr
            deserializeStartAngularVelocityInitializer(const std::string&);

            static
            ParticleModifierPtr
            deserializeColorBySpeedUpdater(const std::string&);

            static
            ParticleModifierPtr
            deserializeColorOverTimeUpdater(const std::string&);

            static
            ParticleModifierPtr
            deserializeForceOverTimeUpdater(const std::string&);

            static
            ParticleModifierPtr
            deserializeSizeBySpeedUpdater(const std::string&);

            static
            ParticleModifierPtr
            deserializeSizeOverTimeUpdater(const std::string&);

            static
            ParticleModifierPtr
            deserializeVelocityOverTimeUpdater(const std::string&);
        };
    }
}