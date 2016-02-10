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
#include "minko/StreamingCommon.hpp"
#include "minko/component/AbstractComponent.hpp"

namespace minko
{
    namespace component
    {
        class MasterLodScheduler :
            public AbstractComponent
        {
        public:
            typedef std::shared_ptr<MasterLodScheduler>         Ptr;

            typedef std::shared_ptr<AbstractComponent>          AbstractComponentPtr;
            typedef std::shared_ptr<scene::Node>                NodePtr;
            typedef std::shared_ptr<AbstractLodScheduler>       AbstractLodSchedulerPtr;

            typedef std::shared_ptr<data::Provider>             ProviderPtr;

            typedef std::shared_ptr<geometry::Geometry>         GeometryPtr;

            typedef std::shared_ptr<render::AbstractTexture>    AbstractTexturePtr;

            typedef std::shared_ptr<file::StreamingOptions>     StreamingOptionsPtr;

        private:
            std::unordered_map<GeometryPtr, ProviderPtr>            _geometryToDataMap;
            std::unordered_map<AbstractTexturePtr, ProviderPtr>     _textureToDataMap;

            StreamingOptionsPtr                                     _streamingOptions;

            Signal<NodePtr, NodePtr, NodePtr>::Slot                 _nodeAddedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot                 _nodeRemovedSlot;

            Signal<NodePtr, NodePtr, AbstractComponentPtr>::Slot    _componentAddedSlot;
            Signal<NodePtr, NodePtr, AbstractComponentPtr>::Slot    _componentRemovedSlot;

            std::vector<AbstractLodSchedulerPtr>                    _lodSchedulers;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new MasterLodScheduler());

                instance->initialize();

                return instance;
            }

            inline
            StreamingOptionsPtr
            streamingOptions() const
            {
                return _streamingOptions;
            }

            void
            invalidateLodRequirement();

            void
            forceUpdate();

            void
            enabled(bool enabled);

            Ptr
            registerGeometry(GeometryPtr geometry, ProviderPtr data);

            void
            unregisterGeometry(GeometryPtr geometry);

            ProviderPtr
            geometryData(GeometryPtr geometry);

            Ptr
            registerTexture(AbstractTexturePtr texture, ProviderPtr data);

            void
            unregisterTexture(AbstractTexturePtr texture);

            ProviderPtr
            textureData(AbstractTexturePtr texture);

            void
            layoutMask(scene::Layout value) override;

        protected:
            void
            targetAdded(NodePtr target) override;

            void
            targetRemoved(NodePtr target) override;

        private:
            MasterLodScheduler();

            void
            initialize();

            void
            addLodScheduler(AbstractLodSchedulerPtr lodScheduler);

            void
            removeLodScheduler(AbstractLodSchedulerPtr lodScheduler);
        };
    }
}