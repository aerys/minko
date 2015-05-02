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

#include "minko/component/MasterLodScheduler.hpp"
#include "minko/data/Provider.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/AbstractTexture.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::geometry;
using namespace minko::render;

MasterLodScheduler::MasterLodScheduler() :
    AbstractComponent(),
    _geometryToDataMap(),
    _textureToDataMap()
{
}

MasterLodScheduler::Ptr
MasterLodScheduler::registerGeometry(Geometry::Ptr geometry, Provider::Ptr data)
{
    _geometryToDataMap.insert(std::make_pair(geometry, data));

    return std::static_pointer_cast<MasterLodScheduler>(shared_from_this());
}

Provider::Ptr
MasterLodScheduler::geometryData(Geometry::Ptr geometry)
{
    auto dataIt = _geometryToDataMap.find(geometry);

    return dataIt != _geometryToDataMap.end() ? dataIt->second : nullptr;
}

MasterLodScheduler::Ptr
MasterLodScheduler::registerTexture(AbstractTexture::Ptr texture, Provider::Ptr data)
{
    _textureToDataMap.insert(std::make_pair(texture, data));

    return std::static_pointer_cast<MasterLodScheduler>(shared_from_this());
}

Provider::Ptr
MasterLodScheduler::textureData(AbstractTexture::Ptr texture)
{
    auto dataIt = _textureToDataMap.find(texture);

    return dataIt != _textureToDataMap.end() ? dataIt->second : nullptr;
}
