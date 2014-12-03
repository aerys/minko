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

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::scene;

TEST_F(PointLightTest, Create)
{
	auto root = Node::create();
	auto n1 = Node::create()->addComponent(PointLight::create(10.f));
	
	ASSERT_TRUE(n1->hasComponent<PointLight>());
	ASSERT_TRUE(n1->component<PointLight>()->diffuse() == 10.0f);
}

TEST_F(PointLightTest, AddLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    lights->addComponent(PointLight::create(.1f, .3f));
    root->addChild(lights);

    ASSERT_TRUE(root->data().hasProperty("pointLight.length"));
    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[0].color"), math::vec3(1.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].specular"), .3f);
}

TEST_F(PointLightTest, RemoveSingleLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");
    auto al = PointLight::create();

    lights->addComponent(al);
    root->addChild(lights);
    lights->removeComponent(al);

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 0);
    ASSERT_FALSE(root->data().hasProperty("pointLight[0].color"));
    ASSERT_FALSE(root->data().hasProperty("pointLight[0].diffuse"));
    ASSERT_FALSE(root->data().hasProperty("pointLight[0].specular"));
}

TEST_F(PointLightTest, AddMultipleLights)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = PointLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].specular"), .2f);

    auto al2 = PointLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[1].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[1].specular"), .4f);

    auto al3 = PointLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 3);
    ASSERT_TRUE(root->data().hasProperty("pointLight[2].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[2].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[2].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[2].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[2].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[2].specular"), .6f);
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

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[0].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].specular"), .4f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[1].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[1].specular"), .6f);
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

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].color"));
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].diffuse"));
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].specular"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_EQ(root->data().get<float>("pointLight[0].diffuse"), .1f);
    ASSERT_EQ(root->data().get<float>("pointLight[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].color"));
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].diffuse"));
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].specular"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_EQ(root->data().get<float>("pointLight[1].diffuse"), .5f);
    ASSERT_EQ(root->data().get<float>("pointLight[1].specular"), .6f);
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

    ASSERT_EQ(root->data().get<uint>("pointLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("pointLight[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("pointLight[1].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("pointLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("pointLight[1].specular"), .4f);
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
            root->data().get<math::vec3>("pointLight[0].position"),
            t,
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}

TEST_F(PointLightTest, Clone)
{
	auto sceneManager = SceneManager::create(MinkoTests::canvas());
	auto root = Node::create()->addComponent(sceneManager);
	auto n1 = Node::create()
		->addComponent(Transform::create(Matrix4x4::create()))
		->addComponent(PointLight::create(10.f));
	

	auto n2 = n1->clone(CloneOption::DEEP);
	n2->component<PointLight>()->diffuse(.1f);
	
	
	root->addChild(n1);
	root->addChild(n2);

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(n1->hasComponent<PointLight>());
	ASSERT_TRUE(n1->component<PointLight>()->diffuse() == 10.0f);
	ASSERT_TRUE(n2->hasComponent<PointLight>());
	ASSERT_TRUE(n2->component<PointLight>()->diffuse() == 0.1f);

	PointLight::Ptr l1 = n1->component<PointLight>();
	PointLight::Ptr l2 = n2->component<PointLight>();	
	ASSERT_TRUE(l1->attenuationCoefficients()->equals(l2->attenuationCoefficients()));

	Vector3::Ptr newCoeffs = Vector3::create(1.5, 1, 1.5);

	l2->attenuationCoefficients(newCoeffs);
	ASSERT_TRUE(l2->attenuationCoefficients()->equals(newCoeffs));
	ASSERT_FALSE(l1->attenuationCoefficients()->equals(l2->attenuationCoefficients()));

	ASSERT_TRUE(l1->position()->equals(l2->position()));

	n2->component<Transform>()->matrix()->prependTranslation(Vector3::create(-5., 0, 2));
	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_FALSE(l1->position()->equals(l2->position()));	
}
