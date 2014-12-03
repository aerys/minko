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

#include "minko/ParticlesCommon.hpp"

#include "minko/render/VertexBuffer.hpp"

namespace minko
{
    namespace render
    {
        class ParticleVertexBuffer :
            public VertexBuffer
        {
        public:
            typedef std::shared_ptr<ParticleVertexBuffer>    Ptr;

        public:
            inline static
            Ptr
            create(std::shared_ptr<render::AbstractContext> context)
            {
                auto vb = std::shared_ptr<ParticleVertexBuffer>(new ParticleVertexBuffer(context));

                vb->initialize();

                return vb;
            }

            void
            initialize();

            // void
            // update(unsigned int    nParticles, unsigned int vertexSize);

            void
            resize(unsigned int nParticles, unsigned int vertexSize);

            void
            resetAttributes()
            {
                /*
                attributes().resize(0);
                vertexSize(0);

                addAttribute("offset", 2, 0);
                addAttribute("position", 3, 2);
                */
            };

        private:
            ParticleVertexBuffer(std::shared_ptr<AbstractContext> context);
        };

    }
}