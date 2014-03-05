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

#include "minko/Common.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/ParticlesCommon.hpp"
#include "minko/extension/AbstractExtension.hpp"
#include "msgpack.hpp"

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

            typedef msgpack::type::tuple<uint, std::vector<float>, std::vector<uint>>               SerializedSampler;
            typedef msgpack::type::tuple<uint, std::vector<float>>                                  SerializedShape;
            typedef msgpack::type::tuple<uint, std::vector<uint>, std::vector<SerializedSampler>>   SerializedModifier;

            typedef std::shared_ptr<particle::modifier::IParticleModifier>                          ParticleModifierPtr;
            typedef std::shared_ptr<particle::shape::EmitterShape>                                  EmitterShapePtr;

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
            deserializeEmitterShape(const SerializedShape&);

            static
            ParticleModifierPtr
            deserializeParticleModifier(const SerializedModifier&);
        };
    }
}