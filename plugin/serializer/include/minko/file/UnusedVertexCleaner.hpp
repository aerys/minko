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
#include "minko/file/AbstractWriterPreprocessor.hpp"
#include "minko/scene/Layout.hpp"

namespace minko
{
    namespace file
    {
        class UnusedVertexCleaner :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<UnusedVertexCleaner>                        Ptr;

            typedef std::shared_ptr<component::Surface>                         SurfacePtr;

            typedef std::shared_ptr<AssetLibrary>                               AssetLibraryPtr;

            typedef std::shared_ptr<geometry::Geometry>                         GeometryPtr;

            typedef std::shared_ptr<scene::Node>                                NodePtr;

        private:
            StatusChangedSignal::Ptr            _statusChanged;
            float                               _progressRate;

        public:
            ~UnusedVertexCleaner() = default;

            static
            Ptr
            create()
            {
                auto instance = Ptr(new UnusedVertexCleaner());

                return instance;
            }

            float
            progressRate() const override
            {
                return _progressRate;
            }

            StatusChangedSignal::Ptr
            statusChanged() override
            {
                return _statusChanged;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary) override;

        private:
            UnusedVertexCleaner();

            void
            processGeometry(GeometryPtr geometry, AssetLibraryPtr assetLibrary);
        };
    }
}
