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

#include "minko/component/LuaScript.hpp"

#include "minko/scene/Node.hpp"
#include "minko/component/LuaScriptManager.hpp"

#include "LuaGlue/LuaGlue.h"

using namespace minko;
using namespace minko::component;

LuaScript::LuaScript(const std::string& name, const std::string& script) :
    _scriptName(name),
    _script(script),
	_state(nullptr),
    _hasStartMethod(false),
    _hasUpdateMethod(false),
    _hasStopMethod(false)
{
}

bool
LuaScript::ready(scene::Node::Ptr target)
{
	if (target->root() && target->root()->hasComponent<LuaScriptManager>())
		return target->root()->component<LuaScriptManager>()->ready(nullptr);
	else
		return false;
}

void
LuaScript::start(scene::Node::Ptr node)
{
	auto stub = _targetToStub.count(node) == 0
		? _targetToStub[node] = new LuaStub()
		: _targetToStub[node];

	if (!_script.empty())
	{
		_state = &(node->root()->component<LuaScriptManager>()->_state);

		auto name = _scriptName.c_str();

		_state->Class<LuaStub>(name)
			.property("running", &LuaStub::running);
		_class = _state->lookupClass(name);
		_class->glue(_state);

		if (!_state->doString(_script))
			printf("err: %s\n", _state->lastError().c_str());
		_script.clear();

		auto c = dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class);
		_hasStartMethod = c->hasMethod("start");
		_hasUpdateMethod = c->hasMethod("update");
		_hasStopMethod = c->hasMethod("stop");

	}

	if (!_hasStartMethod)
		return;

	dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class)->invokeVoidMethod("start", stub, node);
}

void
LuaScript::update(scene::Node::Ptr node)
{
	if (!_hasUpdateMethod)
		return;

	auto stub = _targetToStub[node];

	dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class)->invokeVoidMethod("update", stub, node);
}

void
LuaScript::stop(scene::Node::Ptr node)
{
	auto stub = _targetToStub.count(node) == 0
		? _targetToStub[node] = new LuaStub()
		: _targetToStub[node];
		
    stub->_running = false;

	if (_hasStopMethod)
		dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class)->invokeVoidMethod("stop", stub, node);

	_targetToStub.erase(node);

	delete stub;
}
