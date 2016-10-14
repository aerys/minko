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
	auto sceneManager = SceneManager::create(MinkoTests::canvas());
	auto root = Node::create()->addComponent(sceneManager);
    auto n1Transform = Transform::create();
	auto n1 = Node::create()->addComponent(n1Transform);
	auto n2 = Node::create()->addComponent(Transform::create());

	root->addChild(n1)->addChild(n2);
	// init. modelToWorldMatrix by performing a first frame
	sceneManager->nextFrame(0.0f, 0.0f);

	auto updated1 = false;
	auto _ = n1->data().propertyChanged("modelToWorldMatrix").connect(
		[&](data::Store& c, data::Provider::Ptr p, const data::Provider::PropertyName& propertyName)
		{
			updated1 = true;
		}
	);

	auto updated2 = false;
	auto __ = n2->data().propertyChanged("modelToWorldMatrix").connect(
        [&](data::Store& c, data::Provider::Ptr p, const data::Provider::PropertyName& propertyName)
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
	auto sceneManager = SceneManager::create(MinkoTests::canvas());
	auto root = Node::create()->addComponent(sceneManager);
    auto n1Transform = Transform::create();
	auto n1 = Node::create()->addComponent(n1Transform);
	auto n2 = Node::create()->addComponent(Transform::create());
    auto n3Transform = Transform::create();
	auto n3 = Node::create()->addComponent(n3Transform);

	root->addChild(n1)->addChild(n2);
	n2->addChild(n3);
	// init. modelToWorldMatrix by performing a first frame
	sceneManager->nextFrame(0.0f, 0.0f);

	auto updated1 = false;
	auto _ = n1->data().propertyChanged("modelToWorldMatrix").connect(
        [&](data::Store& c, data::Provider::Ptr p, const data::Provider::PropertyName& propertyName)
		{
			updated1 = true;
		}
	);

	auto updated2 = false;
	auto __ = n3->data().propertyChanged("modelToWorldMatrix").connect(
        [&](data::Store& c, data::Provider::Ptr p, const data::Provider::PropertyName& propertyName)
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
	auto sceneManager = SceneManager::create(MinkoTests::canvas());
	auto root = Node::create()->addComponent(sceneManager);
    auto n1Transform = Transform::create();
	auto n1 = Node::create()->addComponent(n1Transform);
    auto n2 = Node::create()->addComponent(Transform::create());
    auto n3Transform = Transform::create();
    auto n3 = Node::create()->addComponent(n3Transform);

	root->addChild(n1)->addChild(n2);

	// init. modelToWorldMatrix by performing a first frame
	sceneManager->nextFrame(0.0f, 0.0f);

	auto updated1 = false;
	auto _ = n1->data().propertyChanged("modelToWorldMatrix").connect(
        [&](data::Store& c, data::Provider::Ptr p, const data::Provider::PropertyName& propertyName)
		{
			updated1 = true;
		}
	);

	auto updated2 = false;
	auto __ = n3->data().propertyChanged("modelToWorldMatrix").connect(
        [&](data::Store& c, data::Provider::Ptr p, const data::Provider::PropertyName& propertyName)
		{
			updated2 = true;
		}
	);

    n1Transform->matrix(math::translate(n1Transform->matrix(), math::vec3(42.f, 0.f, 0.f)));

	sceneManager->nextFrame(0.0f, 0.0f);

	n2->addChild(n3);
    n3Transform->matrix(math::translate(n3Transform->matrix(), math::vec3(42.f, 0.f, 0.f)));

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(updated1);
	ASSERT_TRUE(updated2);
}

TEST_F(TransformTest, NodeHierarchyTransformIssueWithBlockingNode)
{
	auto sceneManager = SceneManager::create(MinkoTests::canvas());

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
	n1->addComponent(Transform::create(math::translate(math::vec3(-4.f, 0.f, 0.f))));
    n2->addComponent(Transform::create(math::translate(math::vec3(-5.f, 0.f, 0.f))));
    n3->addComponent(Transform::create(math::translate(math::vec3(4.f, 0, 0.f))));
    n4->addComponent(Transform::create(math::translate(math::vec3(5.f, 0.f, 0.f))));

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

    auto zero = math::vec4(0.f, 0.f, 0.f, 1.f);

	ASSERT_EQ((n1->component<Transform>()->matrix() * zero).xyz(), math::vec3(-4.f, -1.f, 0.f));
	ASSERT_EQ((n2->component<Transform>()->matrix() * zero).xyz(), math::vec3(-5.f, 0.f, 0.f));
    ASSERT_EQ((n3->component<Transform>()->modelToWorldMatrix() * zero).xyz(), math::vec3(4.f, 1.f, 0.f));
    ASSERT_EQ((n4->component<Transform>()->modelToWorldMatrix() * zero).xyz(), math::vec3(5.f, 1.f, 0.f));
}

TEST_F(TransformTest, NodeHierarchyTransformIssueWithoutBlockingNode)
{
	auto sceneManager = SceneManager::create(MinkoTests::canvas());

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
        math::translate(math::vec3(-4.f, 0.f, 0.f))
	));

	n2->addComponent(Transform::create(
        math::translate(math::vec3(-5.f, 0.f, 0.f))
	));

	n3->addComponent(Transform::create(
        math::scale(math::vec3(5.f)) * math::translate(math::vec3(4.f, 0.f, 0.f))
	));

    n4->addComponent(Transform::create(
        math::scale(math::vec3(10.f)) * math::translate(math::vec3(5.f, 0.f, 0.f))
	));

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

    auto zero = math::vec4(0.f, 0.f, 0.f, 1.f);

	ASSERT_EQ((n1->component<Transform>()->matrix() * zero).xyz(), math::vec3(-4.f, -1.f, 0.f));
    ASSERT_EQ((n2->component<Transform>()->matrix() * zero).xyz(), math::vec3(-5.f, 0.f, 0.f));
    ASSERT_EQ((n3->component<Transform>()->modelToWorldMatrix() * zero).xyz(), math::vec3(20.f, 1.f, 0.f));
    ASSERT_EQ((n4->component<Transform>()->modelToWorldMatrix() * zero).xyz(), math::vec3(50.f, 1.f, 0.f));
}

TEST_F(TransformTest, RemoveParentTransform)
{
    auto root = Node::create("root");
    auto a = Node::create("a");
    auto b = Node::create("b");
    auto c = Node::create("c");

    root->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.f, 1.f))));

    a->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));
    root->addChild(a);


    b->addComponent(Transform::create(math::translate(math::vec3(0.f, 1.f, 0.f))));
    root->addChild(b);

    c->addComponent(Transform::create(math::translate(math::vec3(2.f, 1.f, 0.f))));
    b->addChild(c);

    b->removeComponent(b->component<Transform>());

    c->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(c->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(2.f, 1.f, 1.f)));
}

TEST_F(TransformTest, Clone)
{
	auto sceneManager = SceneManager::create(MinkoTests::canvas());
	auto root = Node::create()->addComponent(sceneManager);
	auto n1 = Node::create()
		->addComponent(Transform::create(math::mat4()));

	auto n2 = n1->clone(CloneOption::DEEP);

	root->addChild(n1);
	root->addChild(n2);

	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_TRUE(n2->component<Transform>()->matrix() == n1->component<Transform>()->matrix());

    n2->component<Transform>()->matrix(math::translate(math::vec3(-5., 0, 2)) * n2->component<Transform>()->matrix());
	sceneManager->nextFrame(0.0f, 0.0f);

	ASSERT_FALSE(n2->component<Transform>()->matrix() == n1->component<Transform>()->matrix());
}

TEST_F(TransformTest, WrongModelToWorldIssue)
{
    auto sceneManager = SceneManager::create(MinkoTests::canvas());

    auto root = Node::create()->addComponent(sceneManager);
    auto n1 = Node::create("n1")
        ->addComponent(
            Transform::create(
                math::inverse(
                    math::lookAt(
                        math::vec3(42.f, 42.f, 42.f),
                        math::vec3(),
                        math::vec3(0.f, 1.f, 0.f)
                    )
                )
            )
        );

    auto n2 = Node::create("n2")->addComponent(Transform::create());
    auto n21 = Node::create("n21")->addComponent(Transform::create());

    root->addChild(n1);
    root->addChild(n2);
    n2->addChild(n21);

    sceneManager->nextFrame(0.0f, 0.0f);

    ASSERT_EQ(math::vec3(n1->component<Transform>()->matrix() * math::vec4(math::vec3(), 1)), math::vec3(42.f, 42.f, 42.f));
    ASSERT_EQ(n2->component<Transform>()->modelToWorldMatrix(), math::mat4());
    ASSERT_EQ(n21->component<Transform>()->modelToWorldMatrix(), math::mat4());
}

TEST_F(TransformTest, RemoveTransformRow)
{
    auto root = Node::create("root");

    auto n0 = Node::create("n0");
    auto n1 = Node::create("n1");

    auto n00 = Node::create("n00");
    auto n10 = Node::create("n10");

    auto n000 = Node::create("n000");
    auto n100 = Node::create("n100");

    root->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.f, 1.f))));

    n0->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n1->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));

    n00->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n10->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));

    n000->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n100->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));

    root->addChild(n0);
    root->addChild(n1);

    n0->addChild(n00);
    n1->addChild(n10);

    n00->addChild(n000);
    n10->addChild(n100);

    root->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(root->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n0->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));
    ASSERT_EQ(n1->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(1.f, 0.f, 1.f)));

    ASSERT_EQ(n00->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-2.f, 0.f, 1.f)));
    ASSERT_EQ(n10->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n000->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-3.f, 0.f, 1.f)));
    ASSERT_EQ(n100->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));

    n00->removeComponent(n00->component<Transform>());
    n10->removeComponent(n10->component<Transform>());

    root->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(root->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n0->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));
    ASSERT_EQ(n1->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(1.f, 0.f, 1.f)));

    ASSERT_EQ(n000->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-2.f, 0.f, 1.f)));
    ASSERT_EQ(n100->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));
}

TEST_F(TransformTest, RemoveMultipleTransformRow)
{
    auto root = Node::create("root");

    auto n0 = Node::create("n0");
    auto n1 = Node::create("n1");

    auto n00 = Node::create("n00");
    auto n10 = Node::create("n10");

    auto n000 = Node::create("n000");
    auto n100 = Node::create("n100");

    root->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.f, 1.f))));

    n0->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n1->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));

    n00->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n10->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));

    n000->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n100->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));

    root->addChild(n0);
    root->addChild(n1);

    n0->addChild(n00);
    n1->addChild(n10);

    n00->addChild(n000);
    n10->addChild(n100);

    root->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(root->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n0->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));
    ASSERT_EQ(n1->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(1.f, 0.f, 1.f)));

    ASSERT_EQ(n00->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-2.f, 0.f, 1.f)));
    ASSERT_EQ(n10->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n000->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-3.f, 0.f, 1.f)));
    ASSERT_EQ(n100->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));

    n0->removeComponent(n0->component<Transform>());
    n1->removeComponent(n1->component<Transform>());

    n00->removeComponent(n00->component<Transform>());
    n10->removeComponent(n10->component<Transform>());

    root->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(root->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n000->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));
    ASSERT_EQ(n100->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));
}

TEST_F(TransformTest, DiscreteRemoveTransform)
{
    auto root = Node::create("root");

    auto n0 = Node::create("n0");
    auto n1 = Node::create("n1");

    auto n00 = Node::create("n00");
    auto n01 = Node::create("n01");
    auto n10 = Node::create("n10");
    auto n11 = Node::create("n11");

    auto n000 = Node::create("n000");
    auto n100 = Node::create("n100");

    root->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.f, 1.f))));

    n0->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n1->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));

    n00->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n01->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));
    n10->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n11->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));

    n000->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n100->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));

    root->addChild(n0);
    root->addChild(n1);

    n0->addChild(n00);
    n0->addChild(n01);
    n1->addChild(n10);
    n1->addChild(n11);

    n00->addChild(n000);
    n10->addChild(n100);

    n00->removeComponent(n00->component<Transform>());
    n10->removeComponent(n10->component<Transform>());

    root->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(root->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));

    ASSERT_EQ(n0->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 1.f)));
    ASSERT_EQ(n1->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(1.f, 0.f, 1.f)));

    ASSERT_EQ(n01->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));
    ASSERT_EQ(n11->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(2.f, 0.f, 1.f)));

    ASSERT_EQ(n000->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-2.f, 0.f, 1.f)));
    ASSERT_EQ(n100->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 1.f)));
}

TEST_F(TransformTest, EmptyAncestorPath)
{
    auto root = Node::create("root");

    auto n0 = Node::create("n0");
    auto n1 = Node::create("n1");
    auto n2 = Node::create("n2");

    auto n00 = Node::create("n00");
    auto n10 = Node::create("n01");
    auto n20 = Node::create("n20");

    auto n100 = Node::create("n100");
    auto n101 = Node::create("n101");

    n0->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n1->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.f, 0.f))));

    n00->addComponent(Transform::create(math::translate(math::vec3(3.f, 0.f, 0.f))));
    n10->addComponent(Transform::create(math::translate(math::vec3(5.f, 0.f, 0.f))));
    n20->addComponent(Transform::create(math::translate(math::vec3(8.f, 0.f, 0.f))));

    n100->addComponent(Transform::create(math::translate(math::vec3(-1.f, 0.f, 0.f))));
    n101->addComponent(Transform::create(math::translate(math::vec3(1.f, 0.f, 0.f))));

    root->addChild(n0);
    root->addChild(n1);
    root->addChild(n2);

    n0->addChild(n00);
    n1->addChild(n10);
    n2->addChild(n20);

    n10->addChild(n100);
    n10->addChild(n101);

    n0->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(n0->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(-1.f, 0.f, 0.f)));
    ASSERT_EQ(n1->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(0.f, 0.f, 0.f)));

    ASSERT_EQ(n00->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(2.f, 0.f, 0.f)));
    ASSERT_EQ(n10->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(5.f, 0.f, 0.f)));
    ASSERT_EQ(n20->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(8.f, 0.f, 0.f)));

    ASSERT_EQ(n100->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(4.f, 0.f, 0.f)));
    ASSERT_EQ(n101->component<Transform>()->modelToWorldMatrix(), math::translate(math::vec3(6.f, 0.f, 0.f)));
}

TEST_F(TransformTest, UpdateModelToWorldMatrixDuringRemovedSignalPropagation)
{
    auto sceneManager = SceneManager::create(MinkoTests::canvas());

    auto root = Node::create("root")
        ->addComponent(sceneManager)
        ->addComponent(Transform::create());

    auto n0 = Node::create("n0")
        ->addComponent(Transform::create())
        ->addComponent(std::make_shared<UpdateTransformWhenStoppedScript>());

    root->addChild(n0);

    sceneManager->nextFrame(0.f, 0.f);

    root->removeChild(n0);
}
