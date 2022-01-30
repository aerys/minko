/*
Copyright (c) 2022 Aerys

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
        class DrawCallPoolTest : public ::testing::Test
        {
        protected:
            template<typename T>
            void
            testStateBindingToDefaultValueSwap(T                            stateMaterialValue, 
                                               const std::string&           stateName,
                                               const std::string&           effectFile,
                                               std::function<T(DrawCall*)>  valueFunc)
            {
                auto fx = MinkoTests::loadEffect(effectFile);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);

                auto pass = fx->techniques().at("default")[0];

                DrawCallPool pool;
                data::Store rootData;
                data::Store rendererData;
                data::Store targetData;

                auto material = material::Material::create();
                targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

                auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
                targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);

                render::EffectVariables variables{
                    { "materialUuid", material->uuid() },
                    { "geometryUuid", geom->uuid() }
                };

                pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);

                const auto& drawCalls = pool.drawCalls().begin()->second;
                auto drawCall = drawCalls.at(0u).empty() ? drawCalls.at(1u).front() : drawCalls.at(0u).front();
                auto stateDefaultValues = drawCall->pass()->stateBindings().defaultValues;

                material->data()->set(stateName, stateMaterialValue);
                pool.update();

                auto drawCallValue = valueFunc(drawCall);
                auto stateDefaultValue = stateDefaultValues.get<T>(stateName);

                auto hasProperty = drawCall->targetData().hasProperty(stateName);
                ASSERT_TRUE(hasProperty);

                ASSERT_EQ(valueFunc(drawCall), material->data()->get<T>(stateName));

                material->data()->unset(stateName);
                pool.update();

                drawCallValue = valueFunc(drawCall);
                stateDefaultValue = stateDefaultValues.get<T>(stateName);

                ASSERT_EQ(valueFunc(drawCall), stateDefaultValues.get<T>(stateName));
            }

            void
            testStateTargetBindingToDefaultValueSwap(const std::string& effectFile,
                                                     const std::string& stateName,
                                                     TextureSampler     stateMaterialValue,
                                                     const std::string& renderTargetName,
                                                     math::ivec2        renderTargetSize)
            {
                auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
                auto fx = MinkoTests::loadEffect(effectFile, assets);

                ASSERT_NE(fx, nullptr);
                ASSERT_EQ(fx->techniques().size(), 1);
                ASSERT_EQ(fx->techniques().at("default").size(), 1);

                auto pass = fx->techniques().at("default")[0];

                DrawCallPool pool;
                data::Store rootData;
                data::Store rendererData;
                data::Store targetData;

                auto material = material::Material::create();
                targetData.addProvider(material->data(), component::Surface::MATERIAL_COLLECTION_NAME);

                auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
                targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);

                render::EffectVariables variables{
                    { "materialUuid", material->uuid() },
                    { "geometryUuid", geom->uuid() }
                };

                pool.addDrawCalls(fx, "default", variables, rootData, rendererData, targetData);
                auto drawCall = pool.drawCalls().begin()->second.at(0u).front();
                auto stateDefaultValues = drawCall->pass()->stateBindings().defaultValues;

                material->data()->set(stateName, stateMaterialValue);
                pool.update();

                auto hasProperty = drawCall->targetData().hasProperty(stateName);
                ASSERT_TRUE(hasProperty);

                ASSERT_EQ(drawCall->target(), material->data()->get<TextureSampler>(stateName));

                material->data()->unset(stateName);
                pool.update();

                auto states = drawCall->pass()->states();

                ASSERT_EQ(drawCall->target(), stateDefaultValues.get<TextureSampler>(stateName));
                ASSERT_NE(states.target(), States::targetDefaultValue());
                ASSERT_EQ(states.target(), assets->texture(renderTargetName)->sampler());
                ASSERT_NE(assets->texture(renderTargetName), nullptr);
                ASSERT_EQ(assets->texture(renderTargetName)->width(), renderTargetSize[0]);
                ASSERT_EQ(assets->texture(renderTargetName)->height(), renderTargetSize[1]);
            }
        };
    }
}
