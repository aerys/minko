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

#include "minko/Minko.hpp"
#include "minko/MinkoTests.hpp"

#include "gtest/gtest.h"

namespace minko
{
    namespace render
    {
        class DrawCallTest : public ::testing::Test
        {
        protected:
            static
            std::string
            randomString(uint len);

            template<typename T, typename U>
            void
            testMultipleUniformsFromRootData(ProgramInputs::Type inputType, std::function<T()> valueFunc)
            {
                data::Store rootData;
                data::Store rendererData;
                data::Store targetData;
                data::Store defaultValues;

                auto p = data::Provider::create();
                auto numProperties = rand() % 32;
                std::map<std::string, data::Binding> bindings;
                std::vector<ProgramInputs::UniformInput> inputs;

                for (auto i = 0; i < numProperties; ++i)
                {
                    auto propertyName = randomString(10);

                    if (!p->hasProperty(propertyName))
                    {
                        p->set<T>(propertyName, valueFunc());
                        bindings["u" + propertyName] = { propertyName, data::Binding::Source::ROOT };
                        inputs.emplace_back("u" + propertyName, rand(), inputType);
                    }
                }
                rootData.addProvider(p);

                DrawCall drawCall(nullptr, {}, rootData, rendererData, targetData);

                bool uniformIsBound = true;

                for (auto& input : inputs)
                    uniformIsBound = uniformIsBound && drawCall.bindUniform(input, bindings, defaultValues);

                ASSERT_TRUE(uniformIsBound);
                ASSERT_EQ(drawCall.boundBoolUniforms().size(), 0);
                ASSERT_EQ(drawCall.boundIntUniforms().size(), 0);
                ASSERT_EQ(drawCall.boundFloatUniforms().size(), numProperties);
                for (auto i = 0; i < numProperties; ++i)
                {
                    ASSERT_EQ(drawCall.boundFloatUniforms()[i].data, rootData.getUnsafePointer<U>(bindings[inputs[i].name].propertyName));
                    ASSERT_EQ(drawCall.boundFloatUniforms()[i].location, inputs[i].location);
                    ASSERT_EQ(drawCall.boundFloatUniforms()[i].size, 1);
                }
            }
        };
    }
}
