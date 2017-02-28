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
#include "minko/data/Provider.hpp"
#include "minko/file/MeshPartitioner.hpp"
#include "minko/file/POPGeometryWriter.hpp"
#include "minko/file/SurfaceOperator.hpp"

namespace minko
{
    namespace file
    {
        class StreamingOptions :
            public std::enable_shared_from_this<StreamingOptions>
        {
        public:
            typedef std::shared_ptr<StreamingOptions>               Ptr;

            typedef std::shared_ptr<component::Surface>             SurfacePtr;

            typedef std::shared_ptr<geometry::Geometry>             GeometryPtr;

            typedef std::shared_ptr<render::AbstractTexture>        AbstractTexturePtr;

            typedef std::function<int(
                int,
                int,
                int,
                float,
                SurfacePtr
            )>                                                      LodFunction;

            typedef std::function<float(
                int,
                int,
                SurfacePtr,
                const data::Store&,
                const data::Store&,
                const data::Store&
            )>                                                      LodPriorityFunction;

            typedef std::function<float(
                float,
                SurfacePtr
            )>                                                      PopGeometryErrorFunction;

        private:
            typedef std::shared_ptr<scene::Node>                    NodePtr;
            typedef std::shared_ptr<file::AbstractWriter<NodePtr>>  SceneWriter;

            typedef std::function<
                GeometryPtr(const std::string&, GeometryPtr)
            >                                                       POPGeometryFunction;
            typedef std::function<
                AbstractTexturePtr(const std::string&, AbstractTexturePtr)
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

            LodFunction                                             _popGeometryLodFunction;
            LodFunction                                             _streamedTextureLodFunction;

            LodPriorityFunction                                     _popGeometryLodPriorityFunction;
            LodPriorityFunction                                     _streamedTextureLodPriorityFunction;

            PopGeometryErrorFunction                                _popGeometryErrorFunction;

            file::MeshPartitioner::Options                          _meshPartitionerOptions;

            float                                                   _popGeometryPriorityFactor;
            float                                                   _streamedTexturePriorityFactor;

            int                                                     _popGeometryMaxPrecisionLevel;
            int                                                     _streamedTextureMaxMipLevel;

            std::function<void(int, int, int&, int&, int&, int&)>   _popGeometryLodRangeFetchingBoundFunction;
            std::function<void(int, int, int&, int&, int&, int&)>   _streamedTextureLodRangeFetchingBoundFunction;

            bool                                                    _createStreamedTextureOnTheFly;

            bool                                                    _popGeometryLodBlendingEnabled;
            float                                                   _popGeometryLodBlendingPeriod;
            int                                                     _popGeometryLodBlendingMinPrecisionLevel;
            bool                                                    _streamedTextureLodBlendingEnabled;
            float                                                   _streamedTextureLodBlendingPeriod;

            int                                                     _maxNumActiveParsers;

            bool                                                    _requestAbortingEnabled;
            float                                                   _abortableRequestProgressThreshold;

            POPGeometryFunction                                     _popGeometryFunction;
            StreamedTextureFunction                                 _streamedTextureFunction;

            std::vector<data::Provider::PropertyName>               _popGeometryLodDependencyProperties;
            std::vector<data::Provider::PropertyName>               _streamedTextureLodDependencyProperties;

            file::SurfaceOperator                                   _surfaceOperator;

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
            const LodFunction&
            popGeometryLodFunction() const
            {
                return _popGeometryLodFunction;
            }

            inline
            Ptr
            popGeometryLodFunction(const LodFunction& function)
            {
                _popGeometryLodFunction = function;

                return shared_from_this();
            }

            inline
            const LodFunction&
            streamedTextureLodFunction() const
            {
                return _streamedTextureLodFunction;
            }

            inline
            Ptr
            streamedTextureLodFunction(const LodFunction& function)
            {
                _streamedTextureLodFunction = function;

                return shared_from_this();
            }

            inline
            const LodPriorityFunction&
            popGeometryLodPriorityFunction() const
            {
                return _popGeometryLodPriorityFunction;
            }

            inline
            Ptr
            popGeometryLodPriorityFunction(const LodPriorityFunction& function)
            {
                _popGeometryLodPriorityFunction = function;

                return shared_from_this();
            }

            inline
            const LodPriorityFunction&
            streamedTextureLodPriorityFunction() const
            {
                return _streamedTextureLodPriorityFunction;
            }

            inline
            Ptr
            streamedTextureLodPriorityFunction(const LodPriorityFunction& function)
            {
                _streamedTextureLodPriorityFunction = function;

                return shared_from_this();
            }

            inline
            const PopGeometryErrorFunction&
            popGeometryErrorFunction() const
            {
                return _popGeometryErrorFunction;
            }

            inline
            Ptr
            popGeometryErrorFunction(const PopGeometryErrorFunction& function)
            {
                _popGeometryErrorFunction = function;

                return shared_from_this();
            }

            inline
            const file::MeshPartitioner::Options&
            meshPartitionerOptions() const
            {
                return _meshPartitionerOptions;
            }

            inline
            Ptr
            meshPartitionerOptions(const file::MeshPartitioner::Options& value)
            {
                _meshPartitionerOptions = value;

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
            bool
            createStreamedTextureOnTheFly() const
            {
                return _createStreamedTextureOnTheFly;
            }

            inline
            Ptr
            createStreamedTextureOnTheFly(bool value)
            {
                _createStreamedTextureOnTheFly = value;

                return shared_from_this();
            }

            inline
            bool
            popGeometryLodBlendingEnabled() const
            {
                return _popGeometryLodBlendingEnabled;
            }

            inline
            Ptr
            popGeometryLodBlendingEnabled(bool value)
            {
                _popGeometryLodBlendingEnabled = value;

                return shared_from_this();
            }

            inline
            float
            popGeometryLodBlendingPeriod() const
            {
                return _popGeometryLodBlendingPeriod;
            }

            inline
            Ptr
            popGeometryLodBlendingPeriod(float value)
            {
                _popGeometryLodBlendingPeriod = value;

                return shared_from_this();
            }

            inline
            int
            popGeometryLodBlendingMinPrecisionLevel() const
            {
                return _popGeometryLodBlendingMinPrecisionLevel;
            }

            inline
            Ptr
            popGeometryLodBlendingMinPrecisionLevel(int value)
            {
                _popGeometryLodBlendingMinPrecisionLevel = value;

                return shared_from_this();
            }

            inline
            bool
            streamedTextureLodBlendingEnabled() const
            {
                return _streamedTextureLodBlendingEnabled;
            }

            inline
            Ptr
            streamedTextureLodBlendingEnabled(bool value)
            {
                _streamedTextureLodBlendingEnabled = value;

                return shared_from_this();
            }

            inline
            float
            streamedTextureLodBlendingPeriod() const
            {
                return _streamedTextureLodBlendingPeriod;
            }

            inline
            Ptr
            streamedTextureLodBlendingPeriod(float value)
            {
                _streamedTextureLodBlendingPeriod = value;

                return shared_from_this();
            }

            inline
            int
            maxNumActiveParsers() const
            {
                return _maxNumActiveParsers;
            }

            inline
            Ptr
            maxNumActiveParsers(int value)
            {
                _maxNumActiveParsers = value;

                return shared_from_this();
            }

            inline
            bool
            requestAbortingEnabled() const
            {
                return _requestAbortingEnabled;
            }

            inline
            Ptr
            requestAbortingEnabled(bool value)
            {
                _requestAbortingEnabled = value;

                return shared_from_this();
            }

            inline
            float
            abortableRequestProgressThreshold() const
            {
                return _abortableRequestProgressThreshold;
            }

            inline
            Ptr
            abortableRequestProgressThreshold(float value)
            {
                _abortableRequestProgressThreshold = value;

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

            inline
            std::vector<data::Provider::PropertyName>&
            popGeometryLodDependencyProperties()
            {
                return _popGeometryLodDependencyProperties;
            }

            inline
            std::vector<data::Provider::PropertyName>&
            streamedTextureLodDependencyProperties()
            {
                return _streamedTextureLodDependencyProperties;
            }

            inline
            const file::SurfaceOperator&
            surfaceOperator() const
            {
                return _surfaceOperator;
            }

            inline
            Ptr
            surfaceOperator(const file::SurfaceOperator& value)
            {
                _surfaceOperator = value;

                return shared_from_this();
            }

        private:
            StreamingOptions();
        };
    }
}
