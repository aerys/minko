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

#include "minko/component/SceneManager.hpp"

#include "minko/file/AssetLibrary.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/AbstractTexture.hpp"

using namespace minko;
using namespace minko::component;

SceneManager::SceneManager(const std::shared_ptr<render::AbstractContext>& context) :
	_clockStart(clock()),
    _frameId(0),
	_assets(file::AssetLibrary::create(context)),
    _frameBegin(Signal<Ptr>::create()),
    _frameEnd(Signal<Ptr>::create()),
	_cullBegin(Signal<Ptr>::create()),
	_cullEnd(Signal<Ptr>::create()),
	_renderBegin(Signal<Ptr, uint, render::AbstractTexture::Ptr>::create()),
	_renderEnd(Signal<Ptr, uint, render::AbstractTexture::Ptr>::create())
{
}

void
SceneManager::initialize()
{
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &SceneManager::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
    _targetRemovedSlot = targetAdded()->connect(std::bind(
        &SceneManager::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));
}

void
SceneManager::targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
	if (target->root() != target)
        throw std::logic_error("SceneManager must be on the root node only.");
	if (target->components<SceneManager>().size() > 1)
		throw std::logic_error("The same root node cannot have more than one SceneManager.");

    _addedSlot = target->added()->connect(std::bind(
        &SceneManager::addedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
}

void
SceneManager::targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
{
    _addedSlot = nullptr;
}

void
SceneManager::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    if (target == targets()[0])
        throw std::logic_error("SceneManager must be on the root node only.");
}

void
SceneManager::nextFrame()
{
    _frameBegin->execute(shared_from_this());
	_cullBegin->execute(shared_from_this());
	_cullEnd->execute(shared_from_this());
	_renderBegin->execute(shared_from_this(), _frameId, nullptr);
	_renderEnd->execute(shared_from_this(), _frameId, nullptr);
    _frameEnd->execute(shared_from_this());

	++_frameId;
}

void
SceneManager::nextFrame(render::AbstractTexture::Ptr renderTarget)
{
	_frameBegin->execute(shared_from_this());
	_cullBegin->execute(shared_from_this());
	_cullEnd->execute(shared_from_this());
	_renderBegin->execute(shared_from_this(), _frameId, renderTarget);
	_renderEnd->execute(shared_from_this(), _frameId, renderTarget);
    _frameEnd->execute(shared_from_this());

	++_frameId;
}

uint
SceneManager::getTimer() const
{
	return (uint)floorf(1e+3f * float(clock() - _clockStart) / float(CLOCKS_PER_SEC));
}
