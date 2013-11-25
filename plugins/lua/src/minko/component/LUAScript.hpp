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

#pragma once

#include "minko/Common.hpp"

#include "minko/component/AbstractScript.hpp"

struct lua_State;

namespace minko
{
    namespace component
    {
        class LuaScript :
            public AbstractScript
        {
        public:
            typedef std::shared_ptr<LuaScript>  Ptr;

        private:
            typedef std::shared_ptr<scene::Node>    NodePtr;

        private:
            lua_State*      _luaState;
            std::string     _scriptName;

        public:
            static inline
            Ptr
            create(lua_State* luaState, const std::string& name)
            {
                auto script = std::shared_ptr<LuaScript>(new LuaScript(luaState, name));

                script->initialize();

                return script;
            }

        protected:
            virtual
			void
			start(NodePtr target);

			virtual
			void
			update(NodePtr target);

            virtual
            void
            stop(NodePtr target);

        private:
            LuaScript(lua_State* state, const std::string& name) :
                _luaState(state),
                _scriptName(name)
            {

            }

            void
            runScriptMethod(const std::string& methodName, NodePtr target);
        };
    }
}
