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
#include "gtest/gtest.h"

namespace minko
{
    namespace file
    {
        class EffectParserTest :
            public ::testing::Test
        {
        public:
            static
            render::Effect::Ptr
            loadEffect(const std::string& filename);

            template<typename T>
            void
            checkStateBindingWithDefaultValue(const std::string& effectFile, 
                                              const std::string& stateName, 
                                              T defaultValue)
            {
                auto fx = loadEffect(effectFile);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);

                auto stateBindings = fx->techniques().at("default")[0]->stateBindings();
                
                ASSERT_EQ(stateBindings.bindings.size(), 1);
                ASSERT_EQ(stateBindings.bindings.at(stateName).propertyName, "material[${materialUuid}]." + stateName);
                ASSERT_EQ(stateBindings.bindings.at(stateName).source, data::Binding::Source::TARGET);
                ASSERT_TRUE(stateBindings.defaultValues.hasProperty(stateName));
                ASSERT_EQ(stateBindings.defaultValues.get<T>(stateName), defaultValue);
            }

            void
            checkStateBinding(const std::string& filename,
                              const std::string& stateProperty,
                              data::Binding::Source bindingSource = data::Binding::Source::TARGET)
            {
                auto fx = loadEffect(filename);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);
                ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.size(), 1);
                ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at(stateProperty).propertyName, "material[${materialUuid}]." + stateProperty);
                ASSERT_EQ(fx->techniques().at("default")[0]->stateBindings().bindings.at(stateProperty).source, bindingSource);
            }

            template<typename T>
            void
            checkSamplerStateDefaultValue(const std::string& filename,
                                          const std::string& samplerStateProperty,
                                          T                  defaultValue)
            {
                auto fx = loadEffect(filename);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);
                ASSERT_EQ(
                    fx->techniques().at("default")[0]->uniformBindings().defaultValues.get<T>(
                        render::SamplerStates::uniformNameToSamplerStateName(
                            "diffuseMap",
                            samplerStateProperty
                        )
                    ),
                    defaultValue
                );
            }

            void
            checkSamplerStateBinding(const std::string& filename,
                                     const std::string& samplerStateProperty)
            {
                auto fx = loadEffect(filename);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);

                auto uniformBindings = fx->techniques().at("default")[0]->uniformBindings().bindings;

                auto uniformName = render::SamplerStates::uniformNameToSamplerStateName(
                    "diffuseMap",
                    samplerStateProperty
                );

                auto bindingName = render::SamplerStates::uniformNameToSamplerStateBindingName(
                    "diffuseMap",
                    samplerStateProperty
                );

                ASSERT_FALSE(uniformBindings.count(uniformName) == 0);
                ASSERT_EQ(
                    uniformBindings.at(uniformName).propertyName,
                    "material[${materialUuid}]." + bindingName
                );
            }

            template<typename T>
            void
            checkSamplerStateBindingWithDefaultValue(const std::string& filename,
                                                     const std::string& samplerStateProperty,
                                                     T                  defaultValue)
            {
                auto fx = loadEffect(filename);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);

                auto uniformBindingMap = fx->techniques().at("default")[0]->uniformBindings();

                auto uniformName = render::SamplerStates::uniformNameToSamplerStateName(
                    "uDiffuseMap",
                    samplerStateProperty
                );

                auto bindingName = render::SamplerStates::uniformNameToSamplerStateBindingName(
                    "diffuseMap",
                    samplerStateProperty
                );

                // Check binding
                ASSERT_FALSE(uniformBindingMap.bindings.count(uniformName) == 0);
                ASSERT_EQ(
                    uniformBindingMap.bindings.at(uniformName).propertyName,
                    "material[${materialUuid}]." + bindingName
                );

                // Check default value
                ASSERT_TRUE(uniformBindingMap.defaultValues.hasProperty(uniformName));

                ASSERT_EQ(
                    uniformBindingMap.defaultValues.get<T>(uniformName),
                    defaultValue
                );
            }
        };
    }
}
