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

class LuaGlue;
class LuaGlueClassBase;

namespace minko
{
    namespace component
    {
        class LuaScript :
            public AbstractScript
        {
        public:
            typedef std::shared_ptr<LuaScript>  Ptr;

            class LuaStub
            {
                friend class LuaScript;

            public:
                LuaStub() : _running(true) {}

                bool
                running()
                {
                    return _running;
                }

            private:
                bool _running;
            };

        private:
            typedef std::shared_ptr<scene::Node>    NodePtr;


        private:
            std::string                             _scriptName;
            std::string                             _script;

            LuaGlue*                                _state;
            LuaGlueClassBase*                       _class;
            std::unordered_map<NodePtr, LuaStub*>   _targetToStub;
            bool                                    _hasStartMethod;
            bool                                    _hasUpdateMethod;
            bool                                    _hasStopMethod;

        public:
            static inline
            Ptr
            create(const std::string& name, const std::string& script)
            {
                auto s = std::shared_ptr<LuaScript>(new LuaScript(name, script));

                s->initialize();

                return s;
            }

        protected:
			void
			start(NodePtr target) override;

			void
			update(NodePtr target) override;

            void
            stop(NodePtr target) override;

            bool
            ready(NodePtr target) override;

		private:
            LuaScript(const std::string& name, const std::string& script);

            static
            void
            initializeLuaBindings();

        };
    }
}
