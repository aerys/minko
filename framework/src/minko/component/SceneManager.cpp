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

#include "minko/component/SceneManager.hpp"

#include "minko/file/AssetLibrary.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/Store.hpp"
#include "minko/AbstractCanvas.hpp"

using namespace minko;
using namespace minko::component;

SceneManager::SceneManager(const std::shared_ptr<AbstractCanvas>& canvas) :
    _canvas(canvas),
    _frameId(0),
	_time(0.f),
    _assets(file::AssetLibrary::create(canvas->context())),
    _frameBegin(Signal<Ptr, float, float>::create()),
    _frameEnd(Signal<Ptr, float, float>::create()),
	_cullBegin(Signal<Ptr>::create()),
	_cullEnd(Signal<Ptr>::create()),
	_renderBegin(Signal<Ptr, uint, render::AbstractTexture::Ptr>::create()),
	_renderEnd(Signal<Ptr, uint, render::AbstractTexture::Ptr>::create()),
	_data(data::Provider::create())
{
}

void
SceneManager::targetAdded(NodePtr target)
{
	if (target->root() != target)
        throw std::logic_error("SceneManager must be on the root node only.");
	if (target->components<SceneManager>().size() > 1)
		throw std::logic_error("The same root node cannot have more than one SceneManager.");

	target->data().addProvider(_data);
    target->data().addProvider(_canvas->data());

    _addedSlot = target->added().connect(std::bind(
        &SceneManager::addedHandler,
        std::static_pointer_cast<SceneManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
}

void
SceneManager::targetRemoved(NodePtr target)
{
    _addedSlot = nullptr;

	target->data().removeProvider(_data);
    target->data().removeProvider(_canvas->data());
}

void
SceneManager::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    if (target == this->target())
        throw std::logic_error("SceneManager must be on the root node only.");
}

void
SceneManager::nextFrame(float time, float deltaTime, render::AbstractTexture::Ptr renderTarget)
{
    _time = time;
	_data->set("time", _time);

	_frameBegin->execute(std::static_pointer_cast<SceneManager>(shared_from_this()), time, deltaTime);
	_cullBegin->execute(std::static_pointer_cast<SceneManager>(shared_from_this()));
	_cullEnd->execute(std::static_pointer_cast<SceneManager>(shared_from_this()));
	_renderBegin->execute(std::static_pointer_cast<SceneManager>(shared_from_this()), _frameId, renderTarget);
	_renderEnd->execute(std::static_pointer_cast<SceneManager>(shared_from_this()), _frameId, renderTarget);
    _frameEnd->execute(std::static_pointer_cast<SceneManager>(shared_from_this()), time, deltaTime);

	++_frameId;
}
