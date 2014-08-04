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

#include "minko/lua/BulletLuaBindingsCollection.hpp"
#include "LuaCollider.hpp"
#include "LuaColliderData.hpp"
#include "LuaColliderDebug.hpp"
#include "LuaPhysicsWorld.hpp"

using namespace minko;
using namespace minko::lua;
using namespace minko::component;

std::vector<std::function<void(LuaGlue&)>>
BulletLuaBindingsCollection::getBulletBindings()
{
    std::vector<std::function<void(LuaGlue&)>> bindings;

    return BulletLuaBindingsCollection::getBulletBindings(bindings);
}

std::vector<std::function<void(LuaGlue&)>>
BulletLuaBindingsCollection::getBulletBindings(std::vector<std::function<void(LuaGlue&)>> bindings)
{
    bindings.push_back(&bullet::LuaColliderData::bind);
    bindings.push_back(&bullet::LuaCollider::bind);
    bindings.push_back(&bullet::LuaColliderDebug::bind);
    bindings.push_back(&bullet::LuaPhysicsWorld::bind);
    return bindings;
}