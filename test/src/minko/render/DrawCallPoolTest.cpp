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

#include "DrawCallPoolTest.hpp"

using namespace minko;
using namespace minko::render;

TEST_F(DrawCallPoolTest, Constructor)
{
    DrawCallPool drawCallPool;
}

TEST_F(DrawCallPoolTest, UniformDefaultToBindingSwap)
{
    auto fx = MinkoTests::loadEffect("effect/OneUniformBindingAndDefault.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    std::unordered_map<std::string, std::string> variables = { { "geometryUuid", geom->uuid() } };

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);

    auto drawCalls = pool.addDrawCalls(fx, variables, "default", rootData, rendererData, targetData);

    ASSERT_EQ(drawCalls.first->boundFloatUniforms().size(), 1);
    ASSERT_EQ(
        drawCalls.first->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );

    auto p = data::Provider::create();

    p->set("diffuseColor", math::vec4(1.));
    targetData.addProvider(p);

    ASSERT_EQ(drawCalls.first->boundFloatUniforms().size(), 1);
    ASSERT_NE(
        drawCalls.first->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
    ASSERT_EQ(
        drawCalls.first->boundFloatUniforms()[0].data,
        math::value_ptr(targetData.get<math::vec4>("diffuseColor"))
    );
}

TEST_F(DrawCallPoolTest, UniformBindingToDefaultSwap)
{
    auto fx = MinkoTests::loadEffect("effect/OneUniformBindingAndDefault.effect");
    auto pass = fx->techniques().at("default")[0];
    DrawCallPool pool;
    data::Store rootData;
    data::Store rendererData;
    data::Store targetData;
    auto geom = geometry::QuadGeometry::create(MinkoTests::canvas()->context());
    std::unordered_map<std::string, std::string> variables = { { "geometryUuid", geom->uuid() } };

    targetData.addProvider(geom->data(), component::Surface::GEOMETRY_COLLECTION_NAME);
    auto p = data::Provider::create();

    p->set("diffuseColor", math::vec4(1.));
    targetData.addProvider(p);

    auto drawCalls = pool.addDrawCalls(fx, variables, "default", rootData, rendererData, targetData);

    ASSERT_EQ(drawCalls.first->boundFloatUniforms().size(), 1);
    ASSERT_NE(
        drawCalls.first->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
    ASSERT_EQ(
        drawCalls.first->boundFloatUniforms()[0].data,
        math::value_ptr(targetData.get<math::vec4>("diffuseColor"))
    );

    p->unset("diffuseColor");

    ASSERT_EQ(drawCalls.first->boundFloatUniforms().size(), 1);
    ASSERT_EQ(
        drawCalls.first->boundFloatUniforms()[0].data,
        math::value_ptr(pass->uniformBindings().defaultValues.get<math::vec4>("uDiffuseColor"))
    );
}
