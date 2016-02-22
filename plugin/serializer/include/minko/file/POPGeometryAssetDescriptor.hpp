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
#include "minko/file/AbstractAssetDescriptor.hpp"
#include "minko/file/AssetLocation.hpp"

namespace minko
{
    namespace file
    {
        class POPGeometryAssetDescriptor :
            public AbstractAssetDescriptor
        {
        public:
            typedef std::shared_ptr<POPGeometryAssetDescriptor> Ptr;

            struct LodDescriptor
            {
                int level;
                int numIndices;
                int numVertices;
                int dataOffset;
                int dataLength;
            };

            typedef std::vector<LodDescriptor>                  LodDescriptors;

        private:
            AssetLocation   _location;

            LodDescriptors  _lodDescriptors;

        public:
            static
            Ptr
            create()
            {
                return Ptr(new POPGeometryAssetDescriptor());
            }

            Ptr
            location(const AssetLocation& location)
            {
                _location = location;

                return std::static_pointer_cast<POPGeometryAssetDescriptor>(shared_from_this());
            }

            const AssetLocation&
            location() const override
            {
                return _location;
            }

            unsigned int
            numLodDescriptors() const
            {
                return _lodDescriptors.size();
            }

            Ptr
            addLodDescriptor(const LodDescriptor& descriptor)
            {
                _lodDescriptors.push_back(descriptor);

                return std::static_pointer_cast<POPGeometryAssetDescriptor>(shared_from_this());
            }

            const LodDescriptor&
            lodDescriptor(int index) const
            {
                return _lodDescriptors.at(index);
            }

        private:
            POPGeometryAssetDescriptor() = default;
        };
    }
}
