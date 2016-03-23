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
#include "minko/component/AbstractComponent.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
    namespace component
    {
        class ShadowMappingTechnique : public AbstractComponent
        {
        public:
            typedef std::shared_ptr<ShadowMappingTechnique> Ptr;

            enum class Technique
            {
                DEFAULT,
                ESM,
                PCF,
                PCF_POISSON
            };

        private:
            Technique           _technique;
            data::Provider::Ptr _data;

        public:
            inline
            static
            Ptr
            create(Technique technique)
            {
                return std::shared_ptr<ShadowMappingTechnique>(new ShadowMappingTechnique(technique));
            }

        private:
            ShadowMappingTechnique(Technique technique) :
                _technique(technique),
                _data(data::Provider::create())
            {
                _data->set("shadowMappingTechnique", static_cast<int>(technique));
            }

            void
            targetAdded(std::shared_ptr<scene::Node> target) override
            {
                target->data().addProvider(_data);
            }
        };
    }
}
