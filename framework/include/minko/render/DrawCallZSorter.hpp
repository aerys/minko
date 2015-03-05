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
#include "minko/Signal.hpp"
#include "minko/data/Binding.hpp"

namespace minko
{
    namespace data
    {
        class Provider;
    }

    namespace render
    {
        class DrawCallZSorter:
            public std::enable_shared_from_this<DrawCallZSorter>
        {
        public:
            typedef std::shared_ptr<DrawCallZSorter>    Ptr;

        private:
            typedef std::shared_ptr<Signal<DrawCall*>>                                              ZSortNeedSignalPtr;
            typedef Signal<data::Store&, std::shared_ptr<data::Provider>, const std::string&>::Slot PropertyChangedSlot;
            typedef std::unordered_map<std::string, data::Binding::Source>                          PropertyInfos;

        private:
            static const PropertyInfos                                  _rawProperties;

            DrawCall*                                                   _drawcall;

            PropertyChangedSlot                                         _targetPropAddedSlot;
            PropertyChangedSlot                                         _targetPropRemovedSlot;
            PropertyChangedSlot                                         _rendererPropAddedSlot;
            PropertyChangedSlot                                         _rendererPropRemovedSlot;

            std::unordered_map<std::string, PropertyChangedSlot>        _propertyChangedSlots;

            // positional members
            math::vec3                                                  _centerPosition;
            const math::mat4*                                           _modelToWorldMatrix;
            const math::mat4*                                           _worldToScreenMatrix;

        public:
            inline static
            Ptr
            create(DrawCall* drawCall)
            {
                return std::shared_ptr<DrawCallZSorter>(new DrawCallZSorter(drawCall));
            }

            void
            initialize(data::Store& targetData, data::Store& rendererData, data::Store& rootData);

            void
            clear();

            math::vec3
            getEyeSpacePosition() const;

        private:
            DrawCallZSorter(DrawCall* drawcall);

            static
            PropertyInfos
            initializeRawProperties();

            void
                propertyAddedHandler(data::Store&, std::shared_ptr<data::Provider>, const std::string&);

            void
                propertyRemovedHandler(data::Store&, std::shared_ptr<data::Provider>, const std::string&);

            void
            requestZSort();
        };
    }
}