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

#include "minko/StreamingCommon.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/POPGeometryLodScheduler.hpp"
#include "minko/component/TextureLodScheduler.hpp"
#include "minko/deserialize/LodSchedulerDeserializer.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/scene/Node.hpp"
#include "minko/file/AbstractSerializerParser.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::deserialize;
using namespace minko::file;
using namespace minko::scene;

AbstractComponent::Ptr
LodSchedulerDeserializer::deserializePOPGeometryLodScheduler(SceneVersion      version, 
                                                             std::string&      serializedData,
                                                             AssetLibrary::Ptr assetLibrary,
                                                             Dependency::Ptr   dependency)
{
    auto lodScheduler = POPGeometryLodScheduler::create();

    return lodScheduler;
}

AbstractComponent::Ptr
LodSchedulerDeserializer::deserializeTextureLodScheduler(SceneVersion      version, 
                                                         std::string&      serializedData,
                                                         AssetLibrary::Ptr assetLibrary,
                                                         Dependency::Ptr   dependency)
{
    auto lodScheduler = TextureLodScheduler::create(assetLibrary);

    return lodScheduler;
}
