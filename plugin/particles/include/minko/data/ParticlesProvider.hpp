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
#include "minko/material/Material.hpp"

namespace minko
{
    namespace data
    {
        class ParticlesProvider:
            public material::Material
        {
        public:
            typedef std::shared_ptr<ParticlesProvider>  Ptr;

        private:
            typedef std::shared_ptr<math::Vector4>              Vector4Ptr;
            typedef std::shared_ptr<render::AbstractTexture>    AbsTexturePtr;

        public:
            inline static
            Ptr
            create()
            {
                Ptr ptr = std::shared_ptr<ParticlesProvider>(new ParticlesProvider());

                ptr->initialize();

                return ptr;
            }

            Ptr
            diffuseColor(uint);

            Ptr
            diffuseColor(Vector4Ptr);

            Ptr
            diffuseMap(AbsTexturePtr);

            Ptr
            unsetDiffuseMap();

            Ptr
            spritesheetSize(unsigned int numCols, unsigned int numRows);

            Ptr
            unsetSpritesheetSize();

            Ptr
            isInWorldSpace(bool);

            bool
            isInWorldSpace() const;

            Vector4Ptr
            diffuseColor() const;

        private:
            ParticlesProvider();

            void
            initialize();
        };
    }
}