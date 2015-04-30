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
#include "minko/StreamingCommon.hpp"
#include "minko/component/MasterLodScheduler.hpp"
#include "minko/file/POPGeometryWriter.hpp"

namespace minko
{
    class StreamingOptions :
        public std::enable_shared_from_this<StreamingOptions>
    {
    public:
        typedef std::shared_ptr<StreamingOptions>               Ptr;

        typedef std::shared_ptr<component::Surface>             SurfacePtr;

        typedef std::shared_ptr<geometry::Geometry>             GeometryPtr;

        typedef std::shared_ptr<render::AbstractTexture>        AbstractTexturePtr;

    private:
        typedef std::shared_ptr<scene::Node>                    NodePtr;
        typedef std::shared_ptr<file::AbstractWriter<NodePtr>>  SceneWriter;

        typedef std::function<
            std::shared_ptr<geometry::Geometry>(
                std::shared_ptr<geometry::Geometry>
            )
        >                                                       POPGeometryFunction;
        typedef std::function<
            std::shared_ptr<render::AbstractTexture>(
                std::shared_ptr<render::AbstractTexture>
            )
        >                                                       StreamedTextureFunction;

    public:
        static const int                                        MAX_LOD;
        static const int                                        MAX_LOD_RANGE;

    private:
        bool                                                    _disableProgressiveLodFetching;

        bool                                                    _textureStreamingIsActive;
        bool                                                    _geometryStreamingIsActive;

        std::shared_ptr<component::MasterLodScheduler>          _masterLodScheduler;

        file::POPGeometryWriter::RangeFunction                  _popGeometryWriterLodRangeFunction;

        int                                                     _popGeometryErrorToleranceThreshold;

        bool                                                    _storeLodData;

        std::function<int(SurfacePtr)>                          _popGeometryLodFunction;
        std::function<int(SurfacePtr)>                          _streamedTextureLodFunction;

        bool                                                    _mergeSurfacesOnPartitioning;
        bool                                                    _useSharedClusterHierarchyOnPartitioning;
        bool                                                    _applyCrackFreePolicyOnPartitioning;

        float                                                   _popGeometryPriorityFactor;
        float                                                   _streamedTexturePriorityFactor;

        int                                                     _popGeometryMaxPrecisionLevel;
        int                                                     _streamedTextureMaxMipLevel;

        std::function<void(int, int, int&, int&, int&, int&)>   _popGeometryLodRangeFetchingBoundFunction;
        std::function<void(int, int, int&, int&, int&, int&)>   _streamedTextureLodRangeFetchingBoundFunction;

        float                                                   _popGeometryBlendingRange;

        POPGeometryFunction                                     _popGeometryFunction;
        StreamedTextureFunction                                 _streamedTextureFunction;

    public:
        inline static
        Ptr
        create()
        {
            auto streamingOptions = Ptr(new StreamingOptions());

            return streamingOptions;
        }

        inline
        bool
        disableProgressiveLodFetching() const
        {
            return _disableProgressiveLodFetching;
        }

        inline
        Ptr
        disableProgressiveLodFetching(bool value)
        {
            _disableProgressiveLodFetching = value;

            return shared_from_this();
        }

        inline
        bool
        textureStreamingIsActive() const
        {
            return _textureStreamingIsActive;
        }

        inline
        Ptr
        textureStreamingIsActive(bool value)
        {
            _textureStreamingIsActive = value;

            return shared_from_this();
        }

        inline
        bool
        geometryStreamingIsActive() const
        {
            return _geometryStreamingIsActive;
        }

        inline
        Ptr
        geometryStreamingIsActive(bool value)
        {
            _geometryStreamingIsActive = value;

            return shared_from_this();
        }

        std::shared_ptr<component::MasterLodScheduler>
        masterLodScheduler() const
        {
            return _masterLodScheduler;
        }

        inline
        Ptr
        masterLodScheduler(std::shared_ptr<component::MasterLodScheduler> value)
        {
            _masterLodScheduler = value;

            _masterLodScheduler->streamingOptions(shared_from_this());

            return shared_from_this();
        }

        inline
        const file::POPGeometryWriter::RangeFunction&
        popGeometryWriterLodRangeFunction() const
        {
            return _popGeometryWriterLodRangeFunction;
        }

        inline
        Ptr
        popGeometryWriterLodRangeFunction(const file::POPGeometryWriter::RangeFunction& function)
        {
            _popGeometryWriterLodRangeFunction = function;

            return shared_from_this();
        }

        inline
        int
        popGeometryErrorToleranceThreshold() const
        {
            return _popGeometryErrorToleranceThreshold;
        }

        inline
        Ptr
        popGeometryErrorToleranceThreshold(int value)
        {
            _popGeometryErrorToleranceThreshold = value;

            return shared_from_this();
        }

        inline
        bool
        storeLodData() const
        {
            return _storeLodData;
        }

        inline
        Ptr
        storeLodData(bool value)
        {
            _storeLodData = value;

            return shared_from_this();
        }

        inline
        const std::function<int(SurfacePtr)>&
        popGeometryLodFunction() const
        {
            return _popGeometryLodFunction;
        }

        inline
        Ptr
        popGeometryLodFunction(const std::function<int(SurfacePtr)>& function)
        {
            _popGeometryLodFunction = function;

            return shared_from_this();
        }

        inline
        const std::function<int(SurfacePtr)>&
        streamedTextureLodFunction() const
        {
            return _streamedTextureLodFunction;
        }

        inline
        Ptr
        streamedTextureLodFunction(const std::function<int(SurfacePtr)>& function)
        {
            _streamedTextureLodFunction = function;

            return shared_from_this();
        }

        inline
        bool
        mergeSurfacesOnPartitioning() const
        {
            return _mergeSurfacesOnPartitioning;
        }

        inline
        Ptr
        mergeSurfacesOnPartitioning(bool value)
        {
            _mergeSurfacesOnPartitioning = value;

            return shared_from_this();
        }

        inline
        bool
        useSharedClusterHierarchyOnPartitioning() const
        {
            return _useSharedClusterHierarchyOnPartitioning;
        }

        inline
        Ptr
        useSharedClusterHierarchyOnPartitioning(bool value)
        {
            _useSharedClusterHierarchyOnPartitioning = value;

            return shared_from_this();
        }

        inline
        bool
        applyCrackFreePolicyOnPartitioning() const
        {
            return _applyCrackFreePolicyOnPartitioning;
        }

        inline
        Ptr
        applyCrackFreePolicyOnPartitioning(bool value)
        {
            _applyCrackFreePolicyOnPartitioning = value;

            return shared_from_this();
        }

        inline
        float
        popGeometryPriorityFactor() const
        {
            return _popGeometryPriorityFactor;
        }

        inline
        Ptr
        popGeometryPriorityFactor(float value)
        {
            _popGeometryPriorityFactor = value;

            return shared_from_this();
        }

        inline
        float
        streamedTexturePriorityFactor() const
        {
            return _streamedTexturePriorityFactor;
        }

        inline
        Ptr
        streamedTexturePriorityFactor(float value)
        {
            _streamedTexturePriorityFactor = value;

            return shared_from_this();
        }

        inline
        int
        popGeometryMaxPrecisionLevel() const
        {
            return _popGeometryMaxPrecisionLevel;
        }

        inline
        Ptr
        popGeometryMaxPrecisionLevel(int value)
        {
            _popGeometryMaxPrecisionLevel = value;

            return shared_from_this();
        }

        inline
        int
        streamedTextureMaxMipLevel() const
        {
            return _streamedTextureMaxMipLevel;
        }

        inline
        Ptr
        streamedTextureMaxMipLevel(int value)
        {
            _streamedTextureMaxMipLevel = value;

            return shared_from_this();
        }

        inline
        const std::function<void(int, int, int&, int&, int&, int&)>&
        popGeometryLodRangeFetchingBoundFunction() const
        {
            return _popGeometryLodRangeFetchingBoundFunction;
        }

        inline
        Ptr
        popGeometryLodRangeFetchingBoundFunction(const std::function<void(int, int, int&, int&, int&, int&)>& function)
        {
            _popGeometryLodRangeFetchingBoundFunction = function;

            return shared_from_this();
        }

        inline
        const std::function<void(int, int, int&, int&, int&, int&)>&
        streamedTextureLodRangeFetchingBoundFunction() const
        {
            return _streamedTextureLodRangeFetchingBoundFunction;
        }

        inline
        Ptr
        streamedTextureLodRangeFetchingBoundFunction(const std::function<void(int, int, int&, int&, int&, int&)>& function)
        {
            _streamedTextureLodRangeFetchingBoundFunction = function;

            return shared_from_this();
        }

        inline
        float
        popGeometryBlendingRange() const
        {
            return _popGeometryBlendingRange;
        }

        inline
        Ptr
        popGeometryBlendingRange(float value)
        {
            _popGeometryBlendingRange = math::clamp(value, 0.f, 1.f);

            return shared_from_this();
        }

        inline
        const POPGeometryFunction&
        popGeometryFunction() const
        {
            return _popGeometryFunction;
        }

        inline
        Ptr
        popGeometryFunction(const POPGeometryFunction& func)
        {
            _popGeometryFunction = func;

            return shared_from_this();
        }

        inline
        const StreamedTextureFunction&
        streamedTextureFunction() const
        {
            return _streamedTextureFunction;
        }

        inline
        Ptr
        streamedTextureFunction(const StreamedTextureFunction& func)
        {
            _streamedTextureFunction = func;

            return shared_from_this();
        }

    private:
        StreamingOptions();
    };
}
