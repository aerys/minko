/*
Copyright (c) 2016 Aerys

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

#include "minko/component/AbstractLight.hpp"
#include "minko/data/Provider.hpp"
#include "minko/render/TextureSampler.hpp"

namespace minko
{
    namespace component
    {
        class ImageBasedLight :
            public AbstractLight
        {
        public:
            typedef std::shared_ptr<ImageBasedLight> Ptr;

        public:
            static
            Ptr
            create()
            {
                auto instance = Ptr(new ImageBasedLight());

                instance->brightness(1.f);

                return instance;
            }

            float
            diffuse() const
            {
                return provider()->get<float>("diffuse");
            }

            Ptr
            diffuse(float value)
            {
                provider()->set("diffuse", minko::math::clamp(value, 0.f, 1.f));

                return std::static_pointer_cast<ImageBasedLight>(shared_from_this());
            }

            float
            specular() const
            {
                return provider()->get<float>("specular");
            }

            Ptr
            specular(float value)
            {
                provider()->set("specular", minko::math::clamp(value, 0.f, 1.f));

                return std::static_pointer_cast<ImageBasedLight>(shared_from_this());
            }

            render::TextureSampler
            irradianceMap() const
            {
                return provider()->get<render::TextureSampler>("irradianceMap");
            }

            Ptr
            irradianceMap(const render::TextureSampler& value)
            {
                provider()->set("irradianceMap", value);

                return std::static_pointer_cast<ImageBasedLight>(shared_from_this());
            }

            render::TextureSampler
            radianceMap() const
            {
                return provider()->get<render::TextureSampler>("radianceMap");
            }

            Ptr
            radianceMap(const render::TextureSampler& value)
            {
                provider()->set("radianceMap", value);

                return std::static_pointer_cast<ImageBasedLight>(shared_from_this());
            }

            float
            brightness() const
            {
                return provider()->get<float>("brightness");
            }

            Ptr
            brightness(float value)
            {
                provider()->set("brightness", value);

                return std::static_pointer_cast<ImageBasedLight>(shared_from_this());
            }

            float
            orientation() const
            {
                return provider()->get<float>("orientation");
            }

            Ptr
            orientation(float value)
            {
                provider()->set("orientation", value);

                return std::static_pointer_cast<ImageBasedLight>(shared_from_this());
            }

        private:
            ImageBasedLight() :
                AbstractLight("imageBasedLight")
            {
            }
        };
    }
}
