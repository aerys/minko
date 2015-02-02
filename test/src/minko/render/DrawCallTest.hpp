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
        private:
            template<typename T>
            using BoundUniformVectorGetter = std::function<const std::vector<DrawCall::UniformValue<T>>(DrawCall&)>;

        protected:
            static
            std::string
            randomString(uint len);

            template<typename T, typename U>
            void
            testMultipleUniformsFromRootData(ProgramInputs::Type            inputType,
                                             uint                           inputSize,
                                             std::function<T()>             valueFunc,
                                             BoundUniformVectorGetter<U>    uniformsFunc)
            {
                data::Store rootData;
                data::Store rendererData;
                data::Store targetData;
                data::Store defaultValues;

                auto p = data::Provider::create();
                auto numProperties = 1 + std::abs(rand() % 32);
                std::unordered_map<std::string, data::Binding> bindings;
                std::vector<ProgramInputs::UniformInput> inputs;

                for (auto i = 0; i < numProperties; ++i)
                {
                    auto propertyName = randomString(10);

                    if (!p->hasProperty(propertyName))
                    {
                        p->set<T>(propertyName, valueFunc());
                        bindings["u" + propertyName] = { propertyName, data::Binding::Source::ROOT };
                        inputs.emplace_back("u" + propertyName, 1 + std::abs(rand()), 1, inputType);
                    }
                }
                rootData.addProvider(p);

                DrawCall drawCall(0, nullptr, std::unordered_map<Flyweight<std::string>, Flyweight<std::string>>{}, rootData, rendererData, targetData);

                bool uniformIsBound = true;
                for (auto& input : inputs)
                    uniformIsBound = uniformIsBound && drawCall.bindUniform(input, bindings, defaultValues);
                ASSERT_TRUE(uniformIsBound);

                const auto& uniforms = uniformsFunc(drawCall);
                ASSERT_EQ(uniforms.size(), inputs.size());
                for (auto i = 0; i < numProperties; ++i)
                    assertBoundUniform<U>(uniforms, i, inputSize, bindings[inputs[i].name], inputs[i], rootData);
            }

            template<typename T>
            void
            assertBoundUniform(const std::vector<DrawCall::UniformValue<T>>&    boundUniforms,
                               uint                                             index,
                               uint                                             inputSize,
                               const data::Binding&                             binding,
                               const ProgramInputs::UniformInput&               input,
                               const data::Store&                               store)
            {
                ASSERT_EQ(
                    boundUniforms[index].data,
                    store.getUnsafePointer<T>(binding.propertyName)
                );
                ASSERT_EQ(boundUniforms[index].location, input.location);
                ASSERT_EQ(boundUniforms[index].size, inputSize);
            }
        };
    }
}
