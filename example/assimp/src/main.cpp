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
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoJPEG.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint			WINDOW_WIDTH	= 800;
const uint			WINDOW_HEIGHT	= 600;
const std::string	MODEL_FILENAME	= "pirate.dae";

const std::string	LABEL_RUN_START		= "run_start";
const std::string	LABEL_RUN_STOP		= "run_stop";
const std::string	LABEL_IDLE			= "idle";
const std::string	LABEL_WALK_START	= "walk_start";
const std::string	LABEL_WALK_STOP		= "walk_stop";
const std::string	LABEL_PUNCH_START	= "punch_start";
const std::string	LABEL_PUNCH_HIT		= "punch_hit";
const std::string	LABEL_PUNCH_STOP	= "punch_stop";
const std::string	LABEL_KICK_START	= "kick_start";
const std::string	LABEL_KICK_HIT		= "kick_hit";
const std::string	LABEL_KICK_STOP		= "kick_stop";
const std::string	LABEL_STUN_START	= "stun_start";
const std::string	LABEL_STUN_STOP		= "stun_stop";

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


void
printAnimationInfo(AbstractAnimation::Ptr anim);

std::shared_ptr<AbstractAnimation> anim = nullptr;

Signal<AbstractAnimation::Ptr>::Slot started;
Signal<AbstractAnimation::Ptr>::Slot stopped;
Signal<AbstractAnimation::Ptr>::Slot looped;
Signal<AbstractAnimation::Ptr, std::string, uint>::Slot labelHit;

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Assimp", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()
		->registerParser<file::ObjFileASSIMPParser>("obj")
		->registerParser<file::ColladaASSIMPParser>("dae")
        ->registerParser<file::JPEGParser>("jpg");
    sceneManager->assets()->loader()
        ->queue("effect/Basic.effect")
	    ->queue("effect/Phong.effect");

	sceneManager->assets()->loader()->options()
		->skinningFramerate(60)
		->skinningMethod(SkinningMethod::HARDWARE)
		->startAnimation(true)
		->effect(sceneManager->assets()->effect("basic"));

	sceneManager->assets()->loader()
		->queue(MODEL_FILENAME);

	bool beIdle = true;
	bool doPunch = false;
	bool doKick = false;
	bool doWalk = false;
	bool doRun = false;
	bool beStun = false;
	bool reverseAnim = false;
	uint speedId = 0;

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.75f, 0.f), Vector3::create(0.25f, 0.75f, 2.5f))
		))
		->addComponent(PerspectiveCamera::create(
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f)
		);
	root->addChild(camera);

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
        auto model = sceneManager->assets()->symbol(MODEL_FILENAME);

		auto surfaceNodeSet = scene::NodeSet::create(model)
			->descendants(true)
			->where([](scene::Node::Ptr n)
		{
			return n->hasComponent<Surface>();
		});

        root
            ->addComponent(AmbientLight::create())
            ->addComponent(DirectionalLight::create())
            ->addChild(model);

		auto skinnedNodes = scene::NodeSet::create(model)
			->descendants(true)
			->where([](scene::Node::Ptr n){ return n->hasComponent<Skinning>(); });

		auto skinnedNode = !skinnedNodes->nodes().empty()
			? skinnedNodes->nodes().front()
			: nullptr;

		anim = skinnedNode->component<Skinning>()
			->addLabel(LABEL_RUN_START,		0)
			->addLabel(LABEL_RUN_STOP,		800)
			->addLabel(LABEL_IDLE,			900)
			->addLabel(LABEL_WALK_START,	1400)
			->addLabel(LABEL_WALK_STOP,		2300)
			->addLabel(LABEL_PUNCH_START,	2333)
			->addLabel(LABEL_PUNCH_HIT,		2600)
			->addLabel(LABEL_PUNCH_STOP,	3000)
			->addLabel(LABEL_KICK_START,	3033)
			->addLabel(LABEL_KICK_HIT,		3316)
			->addLabel(LABEL_KICK_STOP,		3600)
			->addLabel(LABEL_STUN_START,	3633)
			->addLabel(LABEL_STUN_STOP,		5033);

		started		= anim->started()->connect([](AbstractAnimation::Ptr){ std::cout << "\nanimation started" << std::endl; });
		stopped		= anim->stopped()->connect([](AbstractAnimation::Ptr){ std::cout << "animation stopped" << std::endl; });
		looped		= anim->looped()->connect([](AbstractAnimation::Ptr){ std::cout << "\nanimation looped" << std::endl; });
		labelHit	= anim->labelHit()->connect([](AbstractAnimation::Ptr, std::string name, uint time){ std::cout << "label '" << name << "'\thit at t = " << time << std::endl; });

		printAnimationInfo(anim);
		idle(anim);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	// currently, keyUp events seem not to be fired at the individual key level
	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		if (!anim)
			return;

		if (k->keyIsDown(input::Keyboard::ScanCode::UP))
		{
			beIdle = doPunch = doKick = doWalk = beStun = reverseAnim = false;
			speedId = 0;
			doRun = true;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::DOWN))
		{
			beIdle = doPunch = doRun = doKick = beStun = reverseAnim = false;
			speedId = 0;
			doWalk = true;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::LEFT))
		{
			beIdle = doRun = doKick = doWalk = beStun = reverseAnim = false;
			speedId = 0;
			doPunch = true;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::RIGHT))
		{
			beIdle = doPunch = doRun = doWalk = beStun = reverseAnim = false;
			speedId = 0;
			doKick = true;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::SPACE))
		{
			beIdle = doPunch = doRun = doKick = doWalk = reverseAnim = false;
			speedId = 0;
			beStun = true;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::END))
		{
			doPunch = doRun = doKick = doWalk = beStun = reverseAnim = false;
			speedId = 0;
			beIdle = true;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::_1))
		{
			doPunch = doRun = doKick = doWalk = beStun = beIdle = reverseAnim = false;
			speedId = 1;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::_2))
		{
			doPunch = doRun = doKick = doWalk = beStun = beIdle = reverseAnim = false;
			speedId = 2;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::_3))
		{
			doPunch = doRun = doKick = doWalk = beStun = beIdle = reverseAnim = false;
			speedId = 3;
		}
		else if (k->keyIsDown(input::Keyboard::ScanCode::R))
		{
			doPunch = doRun = doKick = doWalk = beStun = beIdle = false;
			reverseAnim = true;
			speedId = 0;
		}
	});

	auto keyUp = canvas->keyboard()->keyUp()->connect([&](input::Keyboard::Ptr k)
	{
		if (!anim)
			return;

		if (doWalk)
			walk(anim);
		else if (doRun)
			run(anim);
		else if (doKick)
			kick(anim);
		else if (doPunch)
			punch(anim);
		else if (beIdle)
			idle(anim);
		else if (beStun)
			stun(anim);
		else if (reverseAnim)
		{
			anim->isReversed(!anim->isReversed());
			std::cout << "animation is " << (!anim->isReversed() ? "not " : "") << "reversed" << std::endl;
		}
		else if (speedId > 0)
		{
			if (speedId == 1)
			{
				anim->timeFunction([](uint t){ return t / 2; });
				std::cout << "animation's speed is decreased" << std::endl;
			}
			else if (speedId == 2)
			{
				anim->timeFunction([](uint t){ return t; });
				std::cout << "animation is back to normal speed" << std::endl;
			}
			else if (speedId == 3)
			{
				anim->timeFunction([](uint t){ return t * 2; });
				std::cout << "animation's speed is increased" << std::endl;
			}
			speedId = 0;
		}
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();
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

void
printAnimationInfo(AbstractAnimation::Ptr anim)
{
	if (anim == nullptr)
		return;

	std::cout << "Animation labels\n--------------" << std::endl;
	for (uint labelId = 0; labelId < anim->numLabels(); ++labelId)
		std::cout << "\t'" << anim->labelName(labelId) << "'\tat t = " << anim->labelTime(labelId) << std::endl;

	std::cout << "Animation controls\n--------------\n\t[up]\trun\n\t[down]\twalk\n\t[left]\tpunch\n\t[right]\tkick\n\t[space]\tstun\n\t[end]\tidle" << std::endl;
	std::cout << "\t[r]\treverse animation\n\t[1]\tlow speed\n\t[2]\tnormal speed\n\t[3]\thigh speed\n" << std::endl;
}
