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

#include "PointLightTest.hpp"

using namespace minko;
using namespace minko::component;

TEST_F(PointLightTest, Create)
{
    try
    {
        auto al = PointLight::create();
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(PointLightTest, AddLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    lights->addComponent(PointLight::create(.1f, .3f));
    root->addChild(lights);

    ASSERT_TRUE(root->data().hasProperty("pointLights.length"));
    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[0].color"), math::vec3(1.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].specular"), .3f);
}

TEST_F(PointLightTest, RemoveSingleLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");
    auto al = PointLight::create();

    lights->addComponent(al);
    root->addChild(lights);
    lights->removeComponent(al);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 0);
    ASSERT_FALSE(root->data().hasProperty("pointLights[0].color"));
    ASSERT_FALSE(root->data().hasProperty("pointLights[0].diffuse"));
    ASSERT_FALSE(root->data().hasProperty("pointLights[0].specular"));
}

TEST_F(PointLightTest, AddMultipleLights)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = PointLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].specular"), .2f);

    auto al2 = PointLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[1].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[1].specular"), .4f);

    auto al3 = PointLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 3);
    ASSERT_TRUE(root->data().hasProperty("pointLights[2].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[2].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[2].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[2].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[2].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[2].specular"), .6f);
}

TEST_F(PointLightTest, RemoveFirstLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = PointLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = PointLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = PointLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al1);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[0].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].specular"), .4f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[1].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[1].specular"), .6f);
}

TEST_F(PointLightTest, RemoveNthLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = PointLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = PointLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = PointLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al2);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].color"));
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].diffuse"));
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].specular"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(root->data().get<float>("pointLights[0].diffuse"), .1f);
    ASSERT_EQ(root->data().get<float>("pointLights[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].color"));
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].diffuse"));
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].specular"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_EQ(root->data().get<float>("pointLights[1].diffuse"), .5f);
    ASSERT_EQ(root->data().get<float>("pointLights[1].specular"), .6f);
}

TEST_F(PointLightTest, RemoveLastLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = PointLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = PointLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = PointLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al3);

    ASSERT_EQ(root->data().get<uint>("pointLights.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLights[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLights[1].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("pointLights[1].specular"));
    ASSERT_EQ(root->data().get<float>("pointLights[1].specular"), .4f);
}

TEST_F(PointLightTest, TranslateXYZ)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = PointLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto t = math::sphericalRand(100.f);
    lights->addComponent(Transform::create(math::translate(t)));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("pointLights[0].position"),
            t,
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}
