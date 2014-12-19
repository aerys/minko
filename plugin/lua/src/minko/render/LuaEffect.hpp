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

#pragma once

#include "minko/Common.hpp"

#include "minko/render/Effect.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
    namespace render
    {
        class LuaEffect :
            public LuaWrapper
        {
        public:
            static
            void
            bind(LuaGlue& state)
            {
                state.Class<Effect>("Effect")
                    .method("setUniformInt",        &Effect::setUniform<int>)
                    .method("setUniformInt2",        &Effect::setUniform<int, int>)
                    .method("setUniformInt3",        &Effect::setUniform<int, int, int>)
                    .method("setUniformInt4",        &Effect::setUniform<int, int, int, int>)
                    .method("setUniformFloat",        &Effect::setUniform<float>)
                    .method("setUniformFloat2",        &Effect::setUniform<float, float>)
                    .method("setUniformFloat3",        &Effect::setUniform<float, float, float>)
                    .method("setUniformFloat4",        &Effect::setUniform<float, float, float, float>)
                    .method("setUniformVector2",    &Effect::setUniform<math::Vector2::Ptr>)
                    .method("setUniformVector3",    &Effect::setUniform<math::Vector3::Ptr>)
                    .method("setUniformVector4",    &Effect::setUniform<math::Vector4::Ptr>)
                    .method("setUniformMatrix4x4",    &Effect::setUniform<math::Matrix4x4::Ptr>);
            }
        };
    }
}
