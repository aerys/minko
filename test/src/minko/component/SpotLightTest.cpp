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

#include "SpotLightTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::scene;

TEST_F(SpotLightTest, Create)
{
	auto root = Node::create();
	auto n1 = Node::create()->addComponent(SpotLight::create((float)PI * 0.25f, -1.0f, 10.f));
	
	ASSERT_TRUE(n1->hasComponent<SpotLight>());
	ASSERT_TRUE(n1->component<SpotLight>()->diffuse() == 10.0f);
}

TEST_F(SpotLightTest, Clone)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());
	auto root = Node::create()->addComponent(sceneManager);
	auto n1 = Node::create()
		->addComponent(Transform::create(Matrix4x4::create()))
		->addComponent(SpotLight::create((float)PI * 0.25f, -1.0f, 10.f));
	

	auto n2 = n1->clone(CloneOption::DEEP);
	n2->component<SpotLight>()->diffuse(.1f);
	
	
	root->addChild(n1);
	root->addChild(n2);

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(n1->hasComponent<SpotLight>());
	ASSERT_TRUE(n1->component<SpotLight>()->diffuse() == 10.0f);
	ASSERT_TRUE(n2->hasComponent<SpotLight>());
	ASSERT_TRUE(n2->component<SpotLight>()->diffuse() == 0.1f);

	SpotLight::Ptr l1 = n1->component<SpotLight>();
	SpotLight::Ptr l2 = n2->component<SpotLight>();
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

