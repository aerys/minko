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

#include "minko/file/AbstractWriterPreprocessor.hpp"

namespace minko
{
    namespace file
    {
        class VertexColorSampler :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<VertexColorSampler>  Ptr;

        private:
            typedef std::shared_ptr<scene::Node>                                NodePtr;
            typedef std::shared_ptr<AssetLibrary>                               AssetLibraryPtr;

        private:
            StatusChangedSignal::Ptr    _statusChanged;

            float                       _gammaCorrection;

        public:
            ~VertexColorSampler() = default;

            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new VertexColorSampler());

                return instance;
            }

            float
            gammaCorrection() const
            {
                return _gammaCorrection;
            }

            Ptr
            gammaCorrection(float value)
            {
                _gammaCorrection = value;

                return std::static_pointer_cast<VertexColorSampler>(shared_from_this());
            }

            inline
            float
            progressRate() const
            {
                return 1.f;
            }

            inline
            StatusChangedSignal::Ptr
            statusChanged()
            {
                return _statusChanged;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary);

        private:
            VertexColorSampler();

            void
            computeVertexColorAttributes(NodePtr            node,
                                         AssetLibraryPtr    assetLibrary);

            void
            computeVertexColorAttributes(std::shared_ptr<geometry::Geometry>    geometry,
                                         std::shared_ptr<material::Material>    material,
                                         AssetLibraryPtr                        assetLibrary);

            void
            sampleColor(unsigned int                       width,
                        unsigned int                       height,
                        unsigned int                       numComponents,
                        const std::vector<unsigned char>&  textureData,
                        const math::vec2&                  uv,
                        math::vec4&                        color);

            static
            float
            packColor(const math::vec3& color);

            static
            math::vec3
            sRGBToRGB(const math::vec3& color, float gammaCorrection);
        };
    }
}
