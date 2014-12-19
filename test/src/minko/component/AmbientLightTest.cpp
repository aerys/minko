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

#include "minko/MinkoTests.hpp"
#include "AmbientLightTest.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;

using namespace minko;
using namespace minko::component;

TEST_F(AmbientLightTest, Create)
{
	auto root = Node::create();
	auto n1 = Node::create()->addComponent(AmbientLight::create(10.f));
	
	ASSERT_TRUE(n1->hasComponent<AmbientLight>());
	ASSERT_TRUE(n1->component<AmbientLight>()->ambient() == 10.0f);
}

TEST_F(AmbientLightTest, AddLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    lights->addComponent(AmbientLight::create());
    root->addChild(lights);

    ASSERT_TRUE(root->data().hasProperty("ambientLight.length"));
    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[0].color"), math::vec3(1.f));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].ambient"));
    ASSERT_EQ(root->data().get<float>("ambientLight[0].ambient"), .2f);
}

TEST_F(AmbientLightTest, RemoveSingleLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");
    auto al = AmbientLight::create();

    lights->addComponent(al);
    root->addChild(lights);
    lights->removeComponent(al);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 0);
    ASSERT_FALSE(root->data().hasProperty("ambientLight[0].color"));
    ASSERT_FALSE(root->data().hasProperty("ambientLight[0].ambient"));
}

TEST_F(AmbientLightTest, AddMultipleLights)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = AmbientLight::create(.1f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[0].ambient"), .1f);

    auto al2 = AmbientLight::create(.2f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[1].ambient"), .2f);

    auto al3 = AmbientLight::create(.3f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 3);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[2].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[2].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[2].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[2].ambient"), .3f);
}

TEST_F(AmbientLightTest, RemoveFirstLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = AmbientLight::create(.1f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = AmbientLight::create(.2f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = AmbientLight::create(.3f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al1);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[0].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[0].ambient"), .2f);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[1].ambient"), .3f);
}

TEST_F(AmbientLightTest, RemoveNthLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = AmbientLight::create(.1f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = AmbientLight::create(.2f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = AmbientLight::create(.3f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al2);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[0].ambient"), .1f);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[1].ambient"), .3f);
}

TEST_F(AmbientLightTest, RemoveLastLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = AmbientLight::create(.1f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = AmbientLight::create(.2f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = AmbientLight::create(.3f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al3);

    ASSERT_EQ(root->data().get<uint>("ambientLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[0].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[0].ambient"), .1f);
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].color"));
    ASSERT_TRUE(root->data().hasProperty("ambientLight[1].ambient"));
    ASSERT_EQ(root->data().get<math::vec3>("ambientLight[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_EQ(root->data().get<float>("ambientLight[1].ambient"), .2f);
}

TEST_F(AmbientLightTest, Clone)
{
	auto sceneManager = SceneManager::create(MinkoTests::canvas());
	auto root = Node::create()->addComponent(sceneManager);
	auto n1 = Node::create()
		->addComponent(Transform::create(math::mat4()))
		->addComponent(AmbientLight::create(10.f));
	
    auto n2 = n1->clone(CloneOption::DEEP);
    n2->component<AmbientLight>()->ambient(.1f);
	
	root->addChild(n1);
	root->addChild(n2);

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(n1->hasComponent<AmbientLight>());
	ASSERT_TRUE(n1->component<AmbientLight>()->ambient() == 10.0f);
	ASSERT_TRUE(n2->hasComponent<AmbientLight>());
	ASSERT_TRUE(n2->component<AmbientLight>()->ambient() == 0.1f);
}
