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

#include "DirectionalLightTest.hpp"

using namespace minko;
using namespace minko::component;

TEST_F(DirectionalLightTest, Create)
{
    try
    {
        auto al = DirectionalLight::create();
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(DirectionalLightTest, AddLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    lights->addComponent(DirectionalLight::create(.1f, .3f));
    root->addChild(lights);

    ASSERT_TRUE(root->data().hasProperty("directionalLight.length"));
    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].color"), math::vec3(1.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].specular"), .3f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].direction"), math::vec3(0.f, 0.f, -1.f));
}

TEST_F(DirectionalLightTest, RemoveSingleLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");
    auto al = DirectionalLight::create();

    lights->addComponent(al);
    root->addChild(lights);
    lights->removeComponent(al);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 0);
    ASSERT_FALSE(root->data().hasProperty("directionalLight[0].color"));
    ASSERT_FALSE(root->data().hasProperty("directionalLight[0].diffuse"));
    ASSERT_FALSE(root->data().hasProperty("directionalLight[0].specular"));
    ASSERT_FALSE(root->data().hasProperty("directionalLight[0].direction"));
}

TEST_F(DirectionalLightTest, AddMultipleLights)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 1);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].direction"), math::vec3(0.f, 0.f, -1.f));

    auto al2 = DirectionalLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].specular"), .4f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].direction"), math::vec3(0.f, 0.f, -1.f));

    auto al3 = DirectionalLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 3);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[2].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[2].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[2].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[2].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[2].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[2].specular"), .6f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[2].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[2].direction"), math::vec3(0.f, 0.f, -1.f));
}

TEST_F(DirectionalLightTest, RemoveFirstLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = DirectionalLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = DirectionalLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al1);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].specular"), .4f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].direction"), math::vec3(0.f, 0.f, -1.f));

    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].specular"), .6f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].direction"), math::vec3(0.f, 0.f, -1.f));
}

TEST_F(DirectionalLightTest, RemoveNthLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = DirectionalLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = DirectionalLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al2);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].direction"), math::vec3(0.f, 0.f, -1.f));

    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].color"), math::vec3(0.f, 0.f, 1.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].diffuse"), .5f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].specular"), .6f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].direction"), math::vec3(0.f, 0.f, -1.f));
}

TEST_F(DirectionalLightTest, RemoveLastLight)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    auto al2 = DirectionalLight::create(.3f, .4f);
    al2->color(math::vec3(0.f, 1.f, 0.f));
    lights->addComponent(al2);

    auto al3 = DirectionalLight::create(.5f, .6f);
    al3->color(math::vec3(0.f, 0.f, 1.f));
    lights->addComponent(al3);

    lights->removeComponent(al3);

    ASSERT_EQ(root->data().get<int>("directionalLight.length"), 2);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].color"), math::vec3(1.f, 0.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].diffuse"), .1f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[0].specular"), .2f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[0].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[0].direction"), math::vec3(0.f, 0.f, -1.f));

    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].color"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].color"), math::vec3(0.f, 1.f, 0.f));
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].diffuse"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].diffuse"), .3f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].specular"));
    ASSERT_EQ(root->data().get<float>("directionalLight[1].specular"), .4f);
    ASSERT_TRUE(root->data().hasProperty("directionalLight[1].direction"));
    ASSERT_EQ(root->data().get<math::vec3>("directionalLight[1].direction"), math::vec3(0.f, 0.f, -1.f));
}

TEST_F(DirectionalLightTest, RemoveLightThenChangeTargetRoot)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    auto al = DirectionalLight::create();
    lights->addComponent(al);
    lights->removeComponent(al);

    root->addChild(lights);

    ASSERT_FALSE(root->data().hasProperty("directionalLight.length"));
}

TEST_F(DirectionalLightTest, RotateXPi)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::rotate(math::pi<float>(), math::vec3(1.f, 0.f, 0.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
        root->data().get<math::vec3>("directionalLight[0].direction"),
        math::vec3(0.f, 0.f, 1.f),
        math::epsilon<float>()
        ),
        math::bvec3(true)
        );
}

TEST_F(DirectionalLightTest, RotateXHalfPi)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::rotate(math::half_pi<float>(), math::vec3(1.f, 0.f, 0.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("directionalLight[0].direction"),
            math::vec3(0.f, 1.f, 0.f),
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}

TEST_F(DirectionalLightTest, RotateYPi)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::rotate(math::pi<float>(), math::vec3(0.f, 1.f, 0.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("directionalLight[0].direction"),
            math::vec3(0.f, 0.f, 1.f),
            math::epsilon<float>()
       ),
       math::bvec3(true)
    );
}

TEST_F(DirectionalLightTest, RotateYHalfPi)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::rotate(math::half_pi<float>(), math::vec3(0.f, 1.f, 0.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("directionalLight[0].direction"),
            math::vec3(-1.f, 0.f, 0.f),
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}

TEST_F(DirectionalLightTest, RotateZPi)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::rotate(math::pi<float>(), math::vec3(0.f, 0.f, 1.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("directionalLight[0].direction"),
            math::vec3(0.f, 0.f, -1.f),
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}

TEST_F(DirectionalLightTest, RotateZHalfPi)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::rotate(math::half_pi<float>(), math::vec3(0.f, 0.f, 1.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("directionalLight[0].direction"),
            math::vec3(0.f, 0.f, -1.f),
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}

TEST_F(DirectionalLightTest, TranslateXYZ)
{
    auto root = scene::Node::create("root");
    auto lights = scene::Node::create("lights");

    root->addChild(lights);

    auto al1 = DirectionalLight::create(.1f, .2f);
    al1->color(math::vec3(1.f, 0.f, 0.f));
    lights->addComponent(al1);

    lights->addComponent(Transform::create(math::translate(math::sphericalRand(100.f))));
    lights->component<Transform>()->updateModelToWorldMatrix();

    ASSERT_EQ(
        math::epsilonEqual(
            root->data().get<math::vec3>("directionalLight[0].direction"),
            math::vec3(0.f, 0.f, -1.f),
            math::epsilon<float>()
        ),
        math::bvec3(true)
    );
}

TEST_F(DirectionalLightTest, OneCascadeNumDeferredPasses)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create("root", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(renderer)
        ->addComponent(ShadowMappingTechnique::create(ShadowMappingTechnique::Technique::ESM));

    auto light = scene::Node::create()->addComponent(
        DirectionalLight::create()
    );
    light->component<DirectionalLight>()->enableShadowMapping(256, 1);
    root->addChild(light);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto geom = geometry::CubeGeometry::create(MinkoTests::canvas()->context());

    auto s1 = Surface::create(geom, material, fx);
    auto s2 = Surface::create(geom, material, fx);
    auto s3 = Surface::create(geom, material, fx);

    root->addComponent(s1);
    root->addComponent(s2);
    root->addComponent(s3);

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    auto shadowRenderer = light->component<Renderer>();

    ASSERT_EQ(renderer->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer->numDrawCalls(), 5);

    root->removeComponent(s1);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 2);
    ASSERT_EQ(shadowRenderer->numDrawCalls(), 4);

    root->removeComponent(s2);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 1);
    ASSERT_EQ(shadowRenderer->numDrawCalls(), 3);

    root->removeComponent(s3);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer->numDrawCalls(), 0);
}

TEST_F(DirectionalLightTest, TwoCascadesNumDeferredPasses)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create("root", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(renderer)
        ->addComponent(ShadowMappingTechnique::create(ShadowMappingTechnique::Technique::ESM));

    auto light = scene::Node::create()->addComponent(
        DirectionalLight::create()
    );
    light->component<DirectionalLight>()->enableShadowMapping(256, 2);
    root->addChild(light);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto geom = geometry::CubeGeometry::create(MinkoTests::canvas()->context());

    auto s1 = Surface::create(geom, material, fx);
    auto s2 = Surface::create(geom, material, fx);
    auto s3 = Surface::create(geom, material, fx);

    root->addComponent(s1);
    root->addComponent(s2);
    root->addComponent(s3);

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    auto shadowRenderer0 = light->component<Renderer>(0);
    auto shadowRenderer1 = light->component<Renderer>(1);

    ASSERT_EQ(renderer->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 5);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 5);

    root->removeComponent(s1);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 2);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 4);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 4);

    root->removeComponent(s2);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 1);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 3);

    root->removeComponent(s3);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 0);
}

TEST_F(DirectionalLightTest, ThreeCascadesNumDeferredPasses)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create("root", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(renderer)
        ->addComponent(ShadowMappingTechnique::create(ShadowMappingTechnique::Technique::ESM));

    auto light = scene::Node::create()->addComponent(
        DirectionalLight::create()
    );
    light->component<DirectionalLight>()->enableShadowMapping(256, 3);
    root->addChild(light);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto geom = geometry::CubeGeometry::create(MinkoTests::canvas()->context());

    auto s1 = Surface::create(geom, material, fx);
    auto s2 = Surface::create(geom, material, fx);
    auto s3 = Surface::create(geom, material, fx);

    root->addComponent(s1);
    root->addComponent(s2);
    root->addComponent(s3);

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    auto shadowRenderer0 = light->component<Renderer>(0);
    auto shadowRenderer1 = light->component<Renderer>(1);
    auto shadowRenderer2 = light->component<Renderer>(2);

    ASSERT_EQ(renderer->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 5);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 5);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 5);

    root->removeComponent(s1);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 2);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 4);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 4);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 4);

    root->removeComponent(s2);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 1);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 3);

    root->removeComponent(s3);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 0);
}

TEST_F(DirectionalLightTest, FourCascadesNumDeferredPasses)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create("root", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(renderer)
        ->addComponent(ShadowMappingTechnique::create(ShadowMappingTechnique::Technique::ESM));

    auto light = scene::Node::create()->addComponent(
        DirectionalLight::create()
    );
    light->component<DirectionalLight>()->enableShadowMapping(256, 4);
    root->addChild(light);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto geom = geometry::CubeGeometry::create(MinkoTests::canvas()->context());

    auto s1 = Surface::create(geom, material, fx);
    auto s2 = Surface::create(geom, material, fx);
    auto s3 = Surface::create(geom, material, fx);

    root->addComponent(s1);
    root->addComponent(s2);
    root->addComponent(s3);

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    auto shadowRenderer0 = light->component<Renderer>(0);
    auto shadowRenderer1 = light->component<Renderer>(1);
    auto shadowRenderer2 = light->component<Renderer>(2);
    auto shadowRenderer3 = light->component<Renderer>(3);

    ASSERT_EQ(renderer->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 5);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 5);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 5);
    ASSERT_EQ(shadowRenderer3->numDrawCalls(), 5);

    root->removeComponent(s1);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 2);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 4);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 4);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 4);
    ASSERT_EQ(shadowRenderer3->numDrawCalls(), 4);

    root->removeComponent(s2);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 1);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 3);
    ASSERT_EQ(shadowRenderer3->numDrawCalls(), 3);

    root->removeComponent(s3);
    root->component<SceneManager>()->nextFrame(0.f, 0.f);
    ASSERT_EQ(renderer->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer0->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer1->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer2->numDrawCalls(), 0);
    ASSERT_EQ(shadowRenderer3->numDrawCalls(), 0);
}

TEST_F(DirectionalLightTest, ShadowMappingEffect)
{
    auto assets = file::AssetLibrary::create(MinkoTests::canvas()->context());
    auto texture = render::Texture::create(assets->context(), 1, 1, false, true);
    texture->upload();
    assets->texture("shadow-map-tmp", texture);
    assets->texture("shadow-map-tmp-2", texture);

    auto fx = MinkoTests::loadEffect("effect/ShadowMap.effect", assets);

    ASSERT_NE(fx, nullptr);
    ASSERT_EQ(fx->techniques().size(), 8);

    for (int i = 0; i < 4; ++i)
    {
        const auto& technique = fx->techniques().at("shadow-map-cascade" + std::to_string(i));

        ASSERT_EQ(technique.size(), 1);
        ASSERT_TRUE(technique[0]->macroBindings().defaultValues.hasProperty("SHADOW_CASCADE_INDEX"));
        ASSERT_TRUE(technique[0]->macroBindings().types.count("SHADOW_CASCADE_INDEX") != 0);
        ASSERT_EQ(technique[0]->macroBindings().types["SHADOW_CASCADE_INDEX"], data::MacroBindingMap::MacroType::INT);
        ASSERT_EQ(technique[0]->macroBindings().defaultValues.get<int>("SHADOW_CASCADE_INDEX"), i);
        ASSERT_EQ(technique[0]->states().priority(), render::States::DEFAULT_PRIORITY);
    }
}

TEST_F(DirectionalLightTest, RenderersAndDrawCalls)
{
    auto fx = MinkoTests::loadEffect("effect/Phong.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create("root", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(renderer);

    auto light = scene::Node::create()->addComponent(DirectionalLight::create());
    light->component<DirectionalLight>()->enableShadowMapping(256, 4);
    root->addChild(light);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto geom = geometry::CubeGeometry::create(MinkoTests::canvas()->context());

    root->addComponent(Surface::create(geom, material, fx));

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    int rendererIndex = 0;
    for (auto renderer : light->components<Renderer>())
    {
        auto shadowMappingDepthPass = renderer->effect()->technique(renderer->effectTechnique())[0];

        ASSERT_TRUE(shadowMappingDepthPass->macroBindings().defaultValues.hasProperty("SHADOW_CASCADE_INDEX"));
        ASSERT_TRUE(shadowMappingDepthPass->macroBindings().types.count("SHADOW_CASCADE_INDEX") != 0);
        ASSERT_EQ(shadowMappingDepthPass->macroBindings().types["SHADOW_CASCADE_INDEX"], data::MacroBindingMap::MacroType::INT);
        ASSERT_EQ(shadowMappingDepthPass->macroBindings().defaultValues.get<int>("SHADOW_CASCADE_INDEX"), rendererIndex);
        ASSERT_EQ(shadowMappingDepthPass->states().priority(), render::States::DEFAULT_PRIORITY);

        const auto& drawCalls = renderer->drawCallPool().drawCalls();
        int* depthTarget = shadowMappingDepthPass->states().target().id;

        ASSERT_EQ(depthTarget, nullptr);
        ASSERT_EQ(drawCalls.count(std::tuple<float, int*>(render::States::DEFAULT_PRIORITY, depthTarget)), 1);

        ++rendererIndex;
    }
}
