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

#include "minko/component/AbstractLodScheduler.hpp"
#include "minko/component/MasterLodScheduler.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::scene;

MasterLodScheduler::MasterLodScheduler() :
    AbstractComponent(),
    _geometryToDataMap(),
    _textureToDataMap(),
    _deferredTextureDataSet(),
    _deferredTextureRegistered(DeferredTextureRegisteredSignal::create()),
    _deferredTextureReady(DeferredTextureReadySignal::create())
{
}

void
MasterLodScheduler::invalidateLodRequirement()
{
    for (auto lodScheduler : _lodSchedulers)
        lodScheduler->invalidateLodRequirement();
}

void
MasterLodScheduler::forceUpdate()
{
    for (auto lodScheduler : _lodSchedulers)
        lodScheduler->forceUpdate();
}

void
MasterLodScheduler::enabled(bool enabled)
{
    for (auto lodScheduler : _lodSchedulers)
        lodScheduler->enabled(enabled);
}

MasterLodScheduler::Ptr
MasterLodScheduler::registerGeometry(Geometry::Ptr geometry, Provider::Ptr data)
{
    _geometryToDataMap.insert(std::make_pair(geometry, data));

    return std::static_pointer_cast<MasterLodScheduler>(shared_from_this());
}

void
MasterLodScheduler::unregisterGeometry(Geometry::Ptr geometry)
{
    _geometryToDataMap.erase(geometry);
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

MasterLodScheduler::Ptr
MasterLodScheduler::registerDeferredTexture(ProviderPtr data)
{
    _deferredTextureDataSet.emplace(data);

    deferredTextureRegistered()->execute(
        std::static_pointer_cast<MasterLodScheduler>(shared_from_this()),
        data
    );

    return std::static_pointer_cast<MasterLodScheduler>(shared_from_this());
}

MasterLodScheduler::Ptr
MasterLodScheduler::deferredTextureReady(ProviderPtr                            data,
                                         const std::unordered_set<ProviderPtr>& materialDataSet,
                                         const Flyweight<std::string>&          textureType,
                                         AbstractTexturePtr                     texture)
{
    deferredTextureReady()->execute(
        std::static_pointer_cast<MasterLodScheduler>(shared_from_this()),
        data,
        materialDataSet,
        textureType,
        texture
    );

    return std::static_pointer_cast<MasterLodScheduler>(shared_from_this());
}

void
MasterLodScheduler::unregisterTexture(AbstractTexture::Ptr texture)
{
    _textureToDataMap.erase(texture);
}

Provider::Ptr
MasterLodScheduler::textureData(AbstractTexture::Ptr texture)
{
    auto dataIt = _textureToDataMap.find(texture);

    return dataIt != _textureToDataMap.end() ? dataIt->second : nullptr;
}

void
MasterLodScheduler::layoutMask(scene::Layout value)
{
    AbstractComponent::layoutMask(value);

    for (auto lodScheduler : _lodSchedulers)
        lodScheduler->layoutMask(value);
}

void
MasterLodScheduler::targetAdded(Node::Ptr target)
{
    AbstractComponent::targetAdded(target);

    _nodeAddedSlot = target->added().connect(
        [this](Node::Ptr target,
               Node::Ptr node,
               Node::Ptr parent)
        {
            auto lodSchedulerNodes = NodeSet::create(node)
                ->descendants(true)
                ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<AbstractLodScheduler>(); });

            for (auto lodSchedulerNode : lodSchedulerNodes->nodes())
                for (auto lodScheduler : lodSchedulerNode->components<AbstractLodScheduler>())
                    addLodScheduler(lodScheduler);
        }
    );

    _nodeRemovedSlot = target->removed().connect(
        [this](Node::Ptr target,
               Node::Ptr node,
               Node::Ptr parent)
        {
            auto lodSchedulerNodes = NodeSet::create(node)
                ->descendants(true)
                ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<AbstractLodScheduler>(); });

            for (auto lodSchedulerNode : lodSchedulerNodes->nodes())
                for (auto lodScheduler : lodSchedulerNode->components<AbstractLodScheduler>())
                    removeLodScheduler(lodScheduler);
        }
    );

    _componentAddedSlot = target->componentAdded().connect(
        [this](Node::Ptr                node,
               Node::Ptr                target,
               AbstractComponent::Ptr   component)
        {
            auto lodScheduler = std::dynamic_pointer_cast<AbstractLodScheduler>(component);

            if (lodScheduler)
                addLodScheduler(lodScheduler);
        }
    );

    _componentRemovedSlot = target->componentRemoved().connect(
        [this](Node::Ptr                node,
               Node::Ptr                target,
               AbstractComponent::Ptr   component)
        {
            auto lodScheduler = std::dynamic_pointer_cast<AbstractLodScheduler>(component);

            if (lodScheduler)
                removeLodScheduler(lodScheduler);
        }
    );
}

void
MasterLodScheduler::targetRemoved(Node::Ptr target)
{
    AbstractComponent::targetRemoved(target);

    _nodeAddedSlot = nullptr;
    _nodeRemovedSlot = nullptr;

    _componentAddedSlot = nullptr;
    _componentRemovedSlot = nullptr;
}

void
MasterLodScheduler::initialize()
{
    _streamingOptions = StreamingOptions::create();

    _streamingOptions->masterLodScheduler(std::static_pointer_cast<MasterLodScheduler>(shared_from_this()));
}

void
MasterLodScheduler::addLodScheduler(AbstractLodScheduler::Ptr lodScheduler)
{
    lodScheduler->layoutMask(AbstractComponent::layoutMask());

    _lodSchedulers.push_back(lodScheduler);
}

void
MasterLodScheduler::removeLodScheduler(AbstractLodScheduler::Ptr lodScheduler)
{
    _lodSchedulers.erase(
        std::remove(_lodSchedulers.begin(), _lodSchedulers.end(), lodScheduler),
        _lodSchedulers.end()
    );
}
