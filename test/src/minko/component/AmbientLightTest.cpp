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

#include "AmbientLightTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::scene;

TEST_F(AmbientLightTest, Create)
{
	auto root = Node::create();
	auto n1 = Node::create()->addComponent(AmbientLight::create(10.f));
	
	ASSERT_TRUE(n1->hasComponent<AmbientLight>());
	ASSERT_TRUE(n1->component<AmbientLight>()->ambient() == 10.0f);
}

TEST_F(AmbientLightTest, Clone)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());
	auto root = Node::create()->addComponent(sceneManager);
	auto n1 = Node::create()
		->addComponent(Transform::create(Matrix4x4::create()))
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

