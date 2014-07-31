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

#include "TransformTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;
using namespace minko::scene;

TEST_F(TransformTest, UniqueRootTransform)
{
	auto root = Node::create();
	auto n1 = Node::create()->addComponent(Transform::create());
	auto n2 = Node::create()->addComponent(Transform::create());
	auto n3 = Node::create();

	ASSERT_FALSE(root->hasComponent<Transform::RootTransform>());
	ASSERT_TRUE(n1->hasComponent<Transform::RootTransform>());
	ASSERT_TRUE(n2->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n3->hasComponent<Transform::RootTransform>());

	n3->addChild(n1);
	ASSERT_FALSE(root->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n1->hasComponent<Transform::RootTransform>());
	ASSERT_TRUE(n2->hasComponent<Transform::RootTransform>());
	ASSERT_TRUE(n3->hasComponent<Transform::RootTransform>());

	root->addChild(n3);
	ASSERT_TRUE(root->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n1->hasComponent<Transform::RootTransform>());
	ASSERT_TRUE(n2->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n3->hasComponent<Transform::RootTransform>());

	root->addChild(n2);
	ASSERT_TRUE(root->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n1->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n2->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n3->hasComponent<Transform::RootTransform>());

	auto n4 = Node::create()->addComponent(Transform::create());
	ASSERT_TRUE(n4->hasComponent<Transform::RootTransform>());

	n2->addChild(n4);
	ASSERT_TRUE(root->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n1->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n2->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n3->hasComponent<Transform::RootTransform>());
	ASSERT_FALSE(n4->hasComponent<Transform::RootTransform>());
}

TEST_F(TransformTest, ModelToWorldUpdate)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());
	auto root = Node::create()->addComponent(sceneManager);
    auto n1Transform = Transform::create();
	auto n1 = Node::create()->addComponent(n1Transform);
	auto n2 = Node::create()->addComponent(Transform::create());

	root->addChild(n1)->addChild(n2);
	// init. transform.modelToWorldMatrix by performing a first frame
	sceneManager->nextFrame(0.0f, 0.0f);

	auto updated1 = false;
	auto _ = n1->data()->propertyChanged("transform.modelToWorldMatrix")->connect(
		[&](data::Container::Ptr c, const std::string& propertyName)
		{
			updated1 = true;
		}
	);

	auto updated2 = false;
	auto __ = n2->data()->propertyChanged("transform.modelToWorldMatrix")->connect(
		[&](data::Container::Ptr c, const std::string& propertyName)
		{
			updated2 = true;
		}
	);

    n1Transform->matrix(math::translate(n1Transform->matrix(), math::vec3(1.F, 0.f, 0.f)));
	//n2->component<Transform>()->matrix()->appendTranslation(1.f);

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(updated1);
	ASSERT_FALSE(updated2);
}

TEST_F(TransformTest, ModelToWorldMultipleUpdates)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());
	auto root = Node::create()->addComponent(sceneManager);
    auto n1Transform = Transform::create();
	auto n1 = Node::create()->addComponent(n1Transform);
	auto n2 = Node::create()->addComponent(Transform::create());
    auto n3Transform = Transform::create();
	auto n3 = Node::create()->addComponent(n3Transform);

	root->addChild(n1)->addChild(n2);
	n2->addChild(n3);
	// init. transform.modelToWorldMatrix by performing a first frame
	sceneManager->nextFrame(0.0f, 0.0f);

	auto updated1 = false;
	auto _ = n1->data()->propertyChanged("transform.modelToWorldMatrix")->connect(
		[&](data::Container::Ptr c, const std::string& propertyName)
		{
			updated1 = true;
		}
	);

	auto updated2 = false;
	auto __ = n3->data()->propertyChanged("transform.modelToWorldMatrix")->connect(
		[&](data::Container::Ptr c, const std::string& propertyName)
		{
			updated2 = true;
		}
	);

	n1Transform->matrix(math::translate(n1Transform->matrix(), math::vec3(1.f, 0.f, 0.f)));
    n3Transform->matrix(math::translate(n3Transform->matrix(), math::vec3(1.f, 0.f, 0.f)));

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(updated1);
	ASSERT_TRUE(updated2);
}


TEST_F(TransformTest, ModelToWorldMultipleUpdatesMultipleFrames)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());
	auto root = Node::create()->addComponent(sceneManager);
    auto n1Transform = Transform::create();
	auto n1 = Node::create()->addComponent(n1Transform);
    auto n2 = Node::create()->addComponent(Transform::create());
    auto n3Transform = Transform::create();
    auto n3 = Node::create()->addComponent(n3Transform);

	root->addChild(n1)->addChild(n2);

	// init. transform.modelToWorldMatrix by performing a first frame
	sceneManager->nextFrame(0.0f, 0.0f);

	auto updated1 = false;
	auto _ = n1->data()->propertyChanged("transform.modelToWorldMatrix")->connect(
		[&](data::Container::Ptr c, const std::string& propertyName)
		{
			updated1 = true;
		}
	);

	auto updated2 = false;
	auto __ = n3->data()->propertyChanged("transform.modelToWorldMatrix")->connect(
		[&](data::Container::Ptr c, const std::string& propertyName)
		{
			updated2 = true;
		}
	);

    n1Transform->matrix(math::translate(n1Transform->matrix(), math::vec3(1.f, 0.f, 0.f)));

	sceneManager->nextFrame(0.0f, 0.0f);

	n2->addChild(n3);
    n3Transform->matrix(math::translate(n3Transform->matrix(), math::vec3(1.f, 0.f, 0.f)));

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(updated1);
	ASSERT_TRUE(updated2);
}

TEST_F(TransformTest, NodeHierarchyTransformIssueWithBlockingNode)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto n1 = Node::create("b");
	auto n2 = Node::create("j");
	auto n3 = Node::create("r");
	auto n4 = Node::create("g");
	auto p3 = Node::create("z");
	auto p2 = Node::create("t");
	p2->addComponent(Transform::create());
	auto p1 = Node::create("f");
	//p1->addComponent(Transform::create());
	

	auto n5 = Node::create("cb");
	n5->addComponent(Transform::create());
	n1->addComponent(Transform::create(math::translate(math::mat4(), math::vec3(-4.f, 0.f, 0.f))));
    n2->addComponent(Transform::create(math::translate(math::mat4(), math::vec3(-5.f, 0.f, 0.f))));
    n3->addComponent(Transform::create(math::translate(math::mat4(), math::vec3(4.f, 0, 0.f))));
    n4->addComponent(Transform::create(math::translate(math::mat4(), math::vec3(5.f, 0.f, 0.f))));

	root->addChild(p2);
		p2->addChild(p1);
			p1->addChild(n3);
			p1->addChild(n4);
		p2->addChild(n5);

	root->addChild(p3);
		p3->addChild(n1);
		p3->addChild(n2);

	sceneManager->nextFrame(0.0f, 0.0f);

    n1->component<Transform>()->matrix(math::translate(
        n1->component<Transform>()->matrix(),
        math::vec3(0.f, -1.f, 0.f)
    ));
    p2->component<Transform>()->matrix(math::translate(
        p2->component<Transform>()->matrix(),
        math::vec3(0.f, 1.f, 0.f)
    ));

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_EQ((n1->component<Transform>()->matrix() * math::vec4()).xyz(), math::vec3(-4.f, -1.f, 0.f));
	ASSERT_EQ((n2->component<Transform>()->matrix() * math::vec4()).xyz(), math::vec3(-5.f, 0.f, 0.f));
	ASSERT_EQ((n3->component<Transform>()->modelToWorldMatrix() * math::vec4()).xyz(), math::vec3(4.f, 1.f, 0.f));
	ASSERT_EQ((n4->component<Transform>()->modelToWorldMatrix() * math::vec4()).xyz(), math::vec3(5.f, 1.f, 0.f));
}

TEST_F(TransformTest, NodeHierarchyTransformIssueWithoutBlockingNode)
{
	auto sceneManager = SceneManager::create(MinkoTests::context());

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto n1 = Node::create("b");
	auto n2 = Node::create("j");
	auto n3 = Node::create("r");
	auto n4 = Node::create("g");
	auto p3 = Node::create("z");
	auto p2 = Node::create("t");
	auto p1 = Node::create("f");
	p2->addComponent(Transform::create());

	auto n5 = Node::create("cb");
	n5->addComponent(Transform::create());

	n1->addComponent(Transform::create(
        math::translate(math::mat4(), math::vec3(-4.f, 0.f, 0.f))
	));

	n2->addComponent(Transform::create(
        math::translate(math::mat4(), math::vec3(-5.f, 0.f, 0.f))
	));

	n3->addComponent(Transform::create(
        math::translate(math::scale(math::mat4(), math::vec3(5.f)), math::vec3(4.f, 0.f, 0.f))
	));
	
    n4->addComponent(Transform::create(
        math::translate(math::scale(math::mat4(), math::vec3(10.f)), math::vec3(5.f, 0.f, 0.f))
	));

	root->addChild(p2);
		p2->addChild(p1);
			p1->addChild(n3);
			p1->addChild(n4);
		//p2->addChild(n5);

	root->addChild(p3);
		p3->addChild(n1);
		p3->addChild(n2);

	sceneManager->nextFrame(0.0f, 0.0f);

    n1->component<Transform>()->matrix(math::translate(
        n1->component<Transform>()->matrix(),
        math::vec3(0.f, -1.f, 0.f)
    ));
    p2->component<Transform>()->matrix(math::translate(
        p2->component<Transform>()->matrix(),
        math::vec3(0.f, 1.f, 0.f)
    ));

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_EQ((n1->component<Transform>()->matrix() * math::vec4()).xyz(), math::vec3(-4.f, -1.f, 0.f));
	ASSERT_EQ((n2->component<Transform>()->matrix() * math::vec4()).xyz(), math::vec3(-5.f, 0.f, 0.f));
	ASSERT_EQ((n3->component<Transform>()->modelToWorldMatrix() * math::vec4()).xyz(), math::vec3(4.f, 1.f, 0.f));
	ASSERT_EQ((n4->component<Transform>()->modelToWorldMatrix() * math::vec4()).xyz(), math::vec3(5.f, 1.f, 0.f));
}