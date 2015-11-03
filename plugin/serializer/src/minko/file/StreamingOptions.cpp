/*
Copyright (c) 2013 Aerys

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

#include "minko/StreamingTypes.hpp"
#include "minko/file/StreamingOptions.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;

const int StreamingOptions::MAX_LOD = 32;
const int StreamingOptions::MAX_LOD_RANGE = 32;

StreamingOptions::StreamingOptions() :
    _disableProgressiveLodFetching(false),
    _textureStreamingIsActive(true),
    _geometryStreamingIsActive(true),
    _masterLodScheduler(),
    _popGeometryWriterLodRangeFunction(),
    _popGeometryErrorToleranceThreshold(3),
    _storeLodData(false),
    _popGeometryLodFunction(),
    _streamedTextureLodFunction([](int, std::shared_ptr<Surface>) -> int { return MAX_LOD; }),
    _meshPartitionerOptions(),
    _popGeometryPriorityFactor(1.f),
    _streamedTexturePriorityFactor(1.f),
    _popGeometryMaxPrecisionLevel(16),
    _streamedTextureMaxMipLevel(0),
    _popGeometryLodRangeFetchingBoundFunction(),
    _streamedTextureLodRangeFetchingBoundFunction(),
    _popGeometryBlendingRange(0.f),
    _maxNumActiveParsers(40),
    _popGeometryFunction(),
    _popGeometryLodDependencyProperties{"modelToWorldMatrix"},
    _streamedTextureLodDependencyProperties{"modelToWorldMatrix"},
    _streamedTextureFunction()
{
}
