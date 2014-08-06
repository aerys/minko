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

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoFX.hpp"
#include "minko/MinkoSerializer.hpp"
#include "minko/MinkoASSIMP.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;
using namespace minko::math;

const std::string MODEL_FILENAME = "model/pirate.scene";

const float WIDTH = 1024;
const float HEIGHT = 1024;

const std::string	LABEL_RUN_START = "run_start";
const std::string	LABEL_RUN_STOP = "run_stop";
const std::string	LABEL_IDLE = "idle";
const std::string	LABEL_WALK_START = "walk_start";
const std::string	LABEL_WALK_STOP = "walk_stop";
const std::string	LABEL_PUNCH_START = "punch_start";
const std::string	LABEL_PUNCH_HIT = "punch_hit";
const std::string	LABEL_PUNCH_STOP = "punch_stop";
const std::string	LABEL_KICK_START = "kick_start";
const std::string	LABEL_KICK_HIT = "kick_hit";
const std::string	LABEL_KICK_STOP = "kick_stop";
const std::string	LABEL_STUN_START = "stun_start";
const std::string	LABEL_STUN_STOP = "stun_stop";

AbstractAnimation::Ptr
run(AbstractAnimation::Ptr anim);

AbstractAnimation::Ptr
walk(AbstractAnimation::Ptr anim);

AbstractAnimation::Ptr
kick(AbstractAnimation::Ptr anim);

AbstractAnimation::Ptr
punch(AbstractAnimation::Ptr anim);

AbstractAnimation::Ptr
idle(AbstractAnimation::Ptr anim);

AbstractAnimation::Ptr
stun(AbstractAnimation::Ptr anim);

std::shared_ptr<AbstractAnimation> anim = nullptr;
std::shared_ptr<AbstractAnimation> anim_clone = nullptr;

Signal<AbstractAnimation::Ptr>::Slot started;
Signal<AbstractAnimation::Ptr>::Slot stopped;
Signal<AbstractAnimation::Ptr>::Slot looped;
Signal<AbstractAnimation::Ptr, std::string, uint>::Slot labelHit;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Clone", WIDTH, HEIGHT);

	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets 
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()
		->registerParser<file::SceneParser>("scene")
		->registerParser<file::ColladaParser>("dae")
		->registerParser<file::PNGParser>("png");

	auto fxLoader = file::Loader::create(sceneManager->assets()->loader())
		->queue("effect/Phong.effect")
		->queue("effect/Basic.effect");

	auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr l)
	{
		sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Phong.effect"));
		sceneManager->assets()->loader()->options()->skinningMethod(SkinningMethod::HARDWARE);
		sceneManager->assets()->loader()->options()->disposeTextureAfterLoading(false);
		sceneManager->assets()->loader()->queue(MODEL_FILENAME);
		sceneManager->assets()->loader()->load();
	});

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.f, 0.f), Vector3::create(0.f, .5f, 0.f))
		))
		->addComponent(PerspectiveCamera::create(WIDTH / HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);


	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
		auto sphereGeometry = geometry::SphereGeometry::create(sceneManager->assets()->context(), 30, 30);

		root->addComponent(AmbientLight::create())
			->addComponent(DirectionalLight::create());

		sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);
		sceneManager->assets()->geometry("sphereGeometry", sphereGeometry);
	
		auto pirate = sceneManager->assets()->symbol(MODEL_FILENAME);

		auto animationsParent = Node::create();

		animationsParent->addChild(pirate);

		auto pirate2 = scene::Node::create()->addChild(pirate->clone(CloneOption::DEEP));
		if (!pirate2->hasComponent<Transform>())
			pirate2->addComponent(Transform::create());
		pirate2->component<Transform>()->matrix()->prependTranslation(5, 0, 0);
		animationsParent->addChild(pirate2);

		auto pirate3 = scene::Node::create()->addChild(pirate->clone(CloneOption::DEEP));
		if (!pirate3->hasComponent<Transform>())
			pirate3->addComponent(Transform::create());
		pirate3->component<Transform>()->matrix()->prependTranslation(0, 0, 5);
		animationsParent->addChild(pirate3);

		auto pirate4 = scene::Node::create()->addChild(pirate->clone(CloneOption::DEEP));
		if (!pirate4->hasComponent<Transform>())
			pirate4->addComponent(Transform::create());
		pirate4->component<Transform>()->matrix()->prependTranslation(5, 0, 5);
		animationsParent->addChild(pirate4);

		root->addChild(animationsParent);

		auto animationNodes = scene::NodeSet::create(pirate)
			->descendants(true)
			->where([](scene::Node::Ptr n){ return n->hasComponent<MasterAnimation>(); });

		auto animationNode = !animationNodes->nodes().empty()
			? animationNodes->nodes().front()
			: nullptr;

		anim = animationNode->component<MasterAnimation>();
		anim
			->addLabel(LABEL_RUN_START, 0)
			->addLabel(LABEL_RUN_STOP, 800)
			->addLabel(LABEL_IDLE, 900)
			->addLabel(LABEL_WALK_START, 1400)
			->addLabel(LABEL_WALK_STOP, 2300)
			->addLabel(LABEL_PUNCH_START, 2333)
			->addLabel(LABEL_PUNCH_HIT, 2600)
			->addLabel(LABEL_PUNCH_STOP, 3000)
			->addLabel(LABEL_KICK_START, 3033)
			->addLabel(LABEL_KICK_HIT, 3316)
			->addLabel(LABEL_KICK_STOP, 3600)
			->addLabel(LABEL_STUN_START, 3633)
			->addLabel(LABEL_STUN_STOP, 5033);

		started = anim->started()->connect([](AbstractAnimation::Ptr){ std::cout << "\nanimation started" << std::endl; });
		stopped = anim->stopped()->connect([](AbstractAnimation::Ptr){ std::cout << "animation stopped" << std::endl; });
		looped = anim->looped()->connect([](AbstractAnimation::Ptr){ std::cout << "\nanimation looped" << std::endl; });
		labelHit = anim->labelHit()->connect([](AbstractAnimation::Ptr, std::string name, uint time){ std::cout << "label '" << name << "'\thit at t = " << time << std::endl; });

		

		std::vector<std::shared_ptr<AbstractAnimation>> _animations;

		auto descendants = NodeSet::create(animationNode->parent())->descendants(true);
		for (auto descendant : descendants->nodes())
		{
			if (descendant->hasComponent<AbstractAnimation>())
			{
				_animations.push_back(descendant->component<AbstractAnimation>());
			}
		}

		auto anims = _animations;


		auto animationNodes_clone = scene::NodeSet::create(pirate2)
			->descendants(true)
			->where([](scene::Node::Ptr n){ return n->hasComponent<MasterAnimation>(); });

		auto animationNode_clone = !animationNodes_clone->nodes().empty()
			? animationNodes_clone->nodes().front()
			: nullptr;

		anim_clone = animationNode_clone->component<MasterAnimation>();
		anim_clone
			->addLabel(LABEL_RUN_START, 0)
			->addLabel(LABEL_RUN_STOP, 800)
			->addLabel(LABEL_IDLE, 900)
			->addLabel(LABEL_WALK_START, 1400)
			->addLabel(LABEL_WALK_STOP, 2300)
			->addLabel(LABEL_PUNCH_START, 2333)
			->addLabel(LABEL_PUNCH_HIT, 2600)
			->addLabel(LABEL_PUNCH_STOP, 3000)
			->addLabel(LABEL_KICK_START, 3033)
			->addLabel(LABEL_KICK_HIT, 3316)
			->addLabel(LABEL_KICK_STOP, 3600)
			->addLabel(LABEL_STUN_START, 3633)
			->addLabel(LABEL_STUN_STOP, 5033);

		started = anim->started()->connect([](AbstractAnimation::Ptr){ std::cout << "\nanimation started" << std::endl; });
		stopped = anim->stopped()->connect([](AbstractAnimation::Ptr){ std::cout << "animation stopped" << std::endl; });
		looped = anim->looped()->connect([](AbstractAnimation::Ptr){ std::cout << "\nanimation looped" << std::endl; });
		labelHit = anim->labelHit()->connect([](AbstractAnimation::Ptr, std::string name, uint time){ std::cout << "label '" << name << "'\thit at t = " << time << std::endl; });


		idle(anim);
		//idle(anim_clone);		

		auto meshes = scene::NodeSet::create(sceneManager->assets()->symbol(MODEL_FILENAME))->descendants(false, false)->where([=](scene::Node::Ptr node)
		{
			return node->hasComponent<Surface>();
		});

		
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		auto width = math::clp2(w);
		auto height = math::clp2(h);
	
	});

	auto yaw = 0.f;
	auto pitch = (float)M_PI * .5f;
	auto minPitch = 0.f + 1e-5;
	auto maxPitch = (float)M_PI - 1e-5;
	auto lookAt = Vector3::create(0.f, .8f, 0.f);
	auto distance = 10.f;

	// handle mouse signals
	auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
	{
		distance += (float)v / 2.f;
	});

	Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
	auto cameraRotationXSpeed = 0.f;
	auto cameraRotationYSpeed = 0.f;

	auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
		{
			cameraRotationYSpeed = (float)dx * .01f;
			cameraRotationXSpeed = (float)dy * -.01f;
		});
	});

	auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
	{
		mouseMove = nullptr;
	});

	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		if (!anim)
			return;


		if (k->keyIsDown(input::Keyboard::UP))
		{
			run(anim);
		}
		else if (k->keyIsDown(input::Keyboard::DOWN))
		{
			walk(anim);
		}
		else if (k->keyIsDown(input::Keyboard::LEFT))
		{
			punch(anim);
		}
		else if (k->keyIsDown(input::Keyboard::RIGHT))
		{
			kick(anim);
		}
		else if (k->keyIsDown(input::Keyboard::SPACE))
		{
			stun(anim);
		}
		else if (k->keyIsDown(input::Keyboard::END))
		{
			idle(anim);
		}
		
	});	

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		auto test = root;

		yaw += cameraRotationYSpeed;
		cameraRotationYSpeed *= 0.9f;

		pitch += cameraRotationXSpeed;
		cameraRotationXSpeed *= 0.9f;
		if (pitch > maxPitch)
			pitch = maxPitch;
		else if (pitch < minPitch)
			pitch = minPitch;

		camera->component<Transform>()->matrix()->lookAt(
			lookAt,
			Vector3::create(
			lookAt->x() + distance * cosf(yaw) * sinf(pitch),
			lookAt->y() + distance * cosf(pitch),
			lookAt->z() + distance * sinf(yaw) * sinf(pitch)
			)
			);

		sceneManager->nextFrame(time, deltaTime);		
	});

	fxLoader->load();
	canvas->run();

	return 0;
}

AbstractAnimation::Ptr
run(AbstractAnimation::Ptr anim)
{
	if (!anim)
		return nullptr;

	anim->isLooping(true);

	return anim->setPlaybackWindow(LABEL_RUN_START, LABEL_RUN_STOP)->play();
}

AbstractAnimation::Ptr
walk(AbstractAnimation::Ptr anim)
{
	if (!anim)
		return nullptr;

	anim->isLooping(true);

	return anim
		->setPlaybackWindow(LABEL_WALK_START, LABEL_WALK_STOP)
		->play();
}

AbstractAnimation::Ptr
kick(AbstractAnimation::Ptr anim)
{
	if (!anim)
		return nullptr;

	anim->isLooping(false);

	return anim
		->setPlaybackWindow(LABEL_KICK_START, LABEL_KICK_STOP, true)
		->play();
}

AbstractAnimation::Ptr
punch(AbstractAnimation::Ptr anim)
{
	if (!anim)
		return nullptr;

	anim->isLooping(false);

	return anim
		->setPlaybackWindow(LABEL_PUNCH_START, LABEL_PUNCH_STOP, true)
		->play();
}

AbstractAnimation::Ptr
idle(AbstractAnimation::Ptr anim)
{
	if (!anim)
		return nullptr;

	anim->isLooping(false);

	return anim
		->resetPlaybackWindow()
		->seek(LABEL_IDLE)
		->stop();
}

AbstractAnimation::Ptr
stun(AbstractAnimation::Ptr anim)
{
	if (!anim)
		return nullptr;

	anim->isLooping(true);

	return anim
		->setPlaybackWindow(LABEL_STUN_START, LABEL_STUN_STOP)
		->play();
}


