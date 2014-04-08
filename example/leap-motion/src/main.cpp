#include <ctime>
#include <random>
#include <chrono>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoLeap.hpp"
#include "minko/MinkoSerializer.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::material;
using namespace minko::component;

static const std::string	SCENE_NAME		= "model/airplane_engine_n.scene";
static const std::string	DEFAULT_EFFECT	= "effect/Basic.effect";
static const std::string	HANGAR_TEXTURE	= "texture/hangar.png";

void
explodeModel(float magnitude, float previousMagnitude, Node::Ptr);

Matrix4x4::Ptr
placeCamera(float cameraDistance, float rotation, Matrix4x4::Ptr);

int main(int argc, char** argv)
{
	// FIXME: need proper support of fullscreen
	bool fullscreen = false;
	if (argc >= 2 && strcmp(argv[1], "--fullscreen") == 0)
		fullscreen = true;

	int		windowWidth		= 800;
	int		windowHeight	= 640;
	auto	canvas			= Canvas::create("Minko Example - Leap", windowWidth, windowHeight);

	// scene set-up
	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());

	auto sceneNode	= Node::create("engine")
		->addComponent(Transform::create());
	auto cameraNode	= Node::create("cameraNode")
		->addComponent(Transform::create());

	// Leap Motion-based controls
	auto controller		= input::leap::Controller::create(canvas);

	const float		MIN_DELTA_HAND_DISTANCE	= 5.0f;
	const float		RELATIVE_MOVE_THRESHOLD	= 1.0f;
	const clock_t	START_CLOCK				= clock();

	// frame-persistant variables
	float	relativeMove			= 0.0f;
	auto	swipeDirection			= Vector3::create();

	float	goalExplosionValue		= 1.0f;
	float	goalCosRotation			= 1.0f;
	float	goalSinRotation			= 0.0f;
	float	goalCameraDistance		= 20.0f;

	float	explosionValue			= goalExplosionValue;
	float	cosRotation				= goalCosRotation;
	float	sinRotation				= goalSinRotation;
	float	cameraDistance			= goalCameraDistance;

	float	previousHandDistance	= -1.0f;
	float	previousTime			= 0.0f;
	float	previousExplosionValue	= explosionValue;

	bool	isInProgress			= false;

	auto leapEnterFrame	= controller->enterFrame()->connect([&](input::leap::Controller::Ptr c)
	{
		if (!c->frame()->isValid())
			return;

		const float currentTime = (clock() - START_CLOCK) / (float)CLOCKS_PER_SEC;
		const float deltaTime	= currentTime - previousTime;
		previousTime			= currentTime;

		auto	frame		= c->frame();
		auto	leftHand	= frame->leftmostHand();
		auto	rightHand	= frame->rightmostHand();

		float handDistance	= 0.0f;

		if (frame->numHands() >= 2)
		{
			if (leftHand->isValid() && rightHand->isValid())
				handDistance = (leftHand->palmPosition() - rightHand->palmPosition())->length();

			if (previousHandDistance < 0.0f)
				previousHandDistance = handDistance;

			const float deltaHandDistance	= fabsf(handDistance - previousHandDistance);

			if (deltaHandDistance > MIN_DELTA_HAND_DISTANCE)
			{
				const float moveIncr = handDistance < previousHandDistance ? -10.0f : 10.0f;
				relativeMove += moveIncr * deltaTime;
			}

			previousHandDistance = handDistance;

			const float moveDecr = relativeMove > 0.0f ? 0.5f : -0.5f;
			relativeMove -= moveDecr * deltaTime;

			if (!isInProgress)
			{
				if (relativeMove > RELATIVE_MOVE_THRESHOLD)
				{
					isInProgress		= true;
					goalExplosionValue	= 3.5f;
					relativeMove		= 0.0f;
#ifdef DEBUG
					std::cout << "spread mesh" << std::endl;
#endif // DEBUG
				}
				else if (relativeMove < - RELATIVE_MOVE_THRESHOLD)
				{
					isInProgress		= true;
					goalExplosionValue	= 1.0f;
					relativeMove		= 0.0f;
#ifdef DEBUG
					std::cout << "restore mesh" << std::endl;
#endif // DEBUG
				}
			}
		}
		else
		{
			// consider only the swipe gesture whose direction exhibits the strongest magnitude
			const uint		numGestures			= frame->numGestures();
			Vector3::Ptr	swipeDirection		= Vector3::create();
			Vector3::Ptr	bestSwipeDirection	= Vector3::create();

			for (uint i = 0; i < numGestures; ++i)
			{
				auto gesture = frame->gestureByIndex(i);

				if (gesture->type() == input::leap::Gesture::Type::Swipe &&
					gesture->state() == input::leap::Gesture::State::Start)
				{
					gesture->toSwipeGesture()->direction(swipeDirection);

					if (swipeDirection->lengthSquared() > bestSwipeDirection->lengthSquared())
						bestSwipeDirection->copyFrom(swipeDirection);
				}
			}

#ifdef DEBUG
			if (bestSwipeDirection->lengthSquared() > 0.1f)
				std::cout << "starting swipe direction = " << bestSwipeDirection->toString() << std::endl;
#endif // DEBUG

			if (!isInProgress && bestSwipeDirection->lengthSquared() > 0.1f)
			{
				if (fabsf(swipeDirection->y()) > 0.5f)
				{
					isInProgress		= true;
					goalCameraDistance	= swipeDirection->y() < 0.0f ? 10.0f : 20.0f;
#ifdef DEBUG
					std::cout << "\t-> zoom in/out\tgoal camera distance = " << goalCameraDistance << std::endl;
#endif // DEBUG
				}
				else if (fabsf(swipeDirection->x()) > 0.5f)
				{
					const float goalRotation	= atan2f(goalSinRotation, goalCosRotation)
						+ (swipeDirection->x() < 0.0f ? (float)PI * 0.25f : - (float)PI * 0.25f);

					isInProgress	= true;
					goalCosRotation	= cosf(goalRotation);
					goalSinRotation	= sinf(goalRotation);
#ifdef DEBUG
					std::cout << "\t-> new rotation angle = " << 180.0f * goalRotation / (float)PI << std::endl;
#endif // DEBUG
				}
			}
		}

		const float diffExplosionValue	= goalExplosionValue - explosionValue;
		const float diffCameraDistance	= goalCameraDistance - cameraDistance;
		const float diffCosRotation		= goalCosRotation - cosRotation;
		const float diffSinRotation		= goalSinRotation - sinRotation;

		if (fabsf(diffExplosionValue) < 0.01f &&
			fabsf(diffCameraDistance) < 0.01f &&
			fabsf(diffCosRotation) < 0.01f &&
			fabsf(diffSinRotation) < 0.01f)
		{
#ifdef DEBUG
			if (isInProgress)
				std::cout << "current motion ended" << std::endl;
#endif // DEBUG

			explosionValue	= goalExplosionValue;
			cameraDistance	= goalCameraDistance;
			cosRotation		= goalCosRotation;
			sinRotation		= goalSinRotation;

			isInProgress	= false;
		}

		explosionValue	+= diffExplosionValue	* std::min(1.0f, 2.0f * deltaTime);
		cameraDistance	+= diffCameraDistance	* std::min(1.0f, 2.5f * deltaTime);
		cosRotation		+= diffCosRotation		* std::min(1.0f, 3.0f * deltaTime);
		sinRotation		+= diffSinRotation		* std::min(1.0f, 3.0f * deltaTime);

		explodeModel(
			explosionValue,
			previousExplosionValue,
			sceneNode
		);

		previousExplosionValue	= explosionValue;

		placeCamera(
			cameraDistance,
			atan2f(sinRotation, cosRotation),
			cameraNode->component<Transform>()->matrix()
		);
	});

	// on initialization of the Leap controller
	auto leapConnected	= controller->connected()->connect([](input::leap::Controller::Ptr c)
	{
		c->enableGesture(input::leap::Gesture::Type::ScreenTap);
		c->enableGesture(input::leap::Gesture::Type::Swipe);
#ifdef DEBUG
		std::cout << "Leap controller connected (screentap and swipe gestures enabled)" << std::endl;
#endif // DEBUG

		std::cout << "Leap controls\n-------------" << std::endl;
		std::cout << "\t- Single hand controls\n\t\t- Horizontal swipe\tturn camera\n\t\t- Vertical swipe\tzoom in/out" << std::endl;
		std::cout << "\t- Two hand controls\n\t\t- Brisk spreading motion\texplose model\n\t\t- Brisk shrinking motion\trestore model" << std::endl;
	});


    auto root				= Node::create("root");

	// setup assets
	auto defaultMaterial	= Material::create()
		->set("diffuseColor", Vector4::create(1.f, 1.f, 1.f, 1.f))
		->set("triangleCulling", render::TriangleCulling::NONE);

	sceneManager->assets()->context()->errorsEnabled(true);

	sceneManager->assets()->loader()->options()->generateMipmaps(false);
	sceneManager->assets()->loader()->options()->material(std::static_pointer_cast<Material>(
			Material::create()->set("triangleCulling", render::TriangleCulling::NONE)
	));
	sceneManager->assets()->loader()->options()->materialFunction([&](const std::string&, Material::Ptr)
	{
		return std::static_pointer_cast<Material>(defaultMaterial);
	});

	sceneManager->assets()->loader()->options()
		->registerParser<file::PNGParser>("png")
                ->registerParser<file::SceneParser>("scene");

        sceneManager->assets()->geometry("cube",		geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere",	geometry::SphereGeometry::create(sceneManager->assets()->context()))
                ->geometry("skybox",	geometry::SphereGeometry::create(sceneManager->assets()->context(), 80, 80, true));

        sceneManager->assets()->loader()->queue(HANGAR_TEXTURE);

	sceneManager->assets()->loader()
                ->queue("effect/Phong.effect")
                ->queue("effect/Basic.effect");

	sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Phong.effect"));

	sceneManager->assets()->loader()->queue(SCENE_NAME);

	auto renderer = Renderer::create();
	renderer->backgroundColor(0x7F7F7FFF);

	cameraNode
		->addComponent(PerspectiveCamera::create(windowWidth / (float)windowHeight))
		->addComponent(renderer);

	placeCamera(
		cameraDistance,
		atan2f(sinRotation, cosRotation),
		cameraNode->component<Transform>()->matrix()
		);

	auto dirLight = Node::create("dirLight")
		->addComponent(component::DirectionalLight::create())
		->addComponent(component::Transform::create(
		Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(-1.0f, -1.0f, -1.0f))
		));

	auto pointLight = Node::create("pointLight")
		->addComponent(component::PointLight::create())
		->addComponent(component::Transform::create(
		Matrix4x4::create()->appendTranslation(0.0f, 10.0f, 0.0f)
		));

	pointLight->component<PointLight>()->color()->setTo(1.0f, 1.0f, 1.0f);
	root->addChild(cameraNode);
	root->addChild(dirLight);
	root->addChild(pointLight);

	auto skybox = Node::create("skybox")
		->addComponent(Transform::create(
		Matrix4x4::create()->appendScale(60.0f, 60.0f, 60.0f)
		));

	root->addComponent(sceneManager);

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		sceneNode->addChild(sceneManager->assets()->symbol(SCENE_NAME));

		skybox->addComponent(Surface::create(
				 sceneManager->assets()->geometry("skybox"),
				 BasicMaterial::create()
					 ->diffuseColor(Vector4::create(1.0f, 0.0f, 0.0f, 1.0f))
					 ->diffuseMap(sceneManager->assets()->texture(HANGAR_TEXTURE))
					 ->triangleCulling(render::TriangleCulling::FRONT),
				sceneManager->assets()->effect("effect/Basic.effect")
			));

		root->addChild(dirLight);
		root->addChild(pointLight);
        root->addChild(skybox);
		root->addChild(cameraNode);
		root->addChild(sceneNode);

        // post-processing
		/*
		auto ppRenderer	= Renderer::create();
		auto ppTarget	= render::Texture::create(sceneManager->assets()->context(), 2048, 2048, false, true);
        ppTarget->upload();

		auto ppScene	= Node::create("ppScene")
			->addComponent(ppRenderer)
			->addComponent(Surface::create(
				geometry::QuadGeometry::create(sceneManager->assets()->context()),
				data::Provider::create()
					->set("backbuffer", ppTarget)
					->set("texcoordOffset", Vector2::create(1.0f/2048.f, 1.0f/2048.0f)),
				sceneManager->assets()->effect("effect/fxaa.effect")
			));
		*/

	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		cameraNode->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		sceneManager->nextFrame(time, deltaTime);
	});

	controller->start();
	sceneManager->assets()->loader()->load();

	canvas->run();
	return 0;
}

Matrix4x4::Ptr
placeCamera(float			cameraDistance,
			float			rotation,
			Matrix4x4::Ptr	output = nullptr)
{
	static const float	CAMERA_ANGLE_X	= 35.0f * (float)PI / 180.0f;
	static const float	CAMERA_ANGLE_Y	= 45.0f * (float)PI / 180.0f;
	static const float	CAMERA_MIN_DIST	= 10.0f;
	static const float	CAMERA_MAX_DIST	= 20.0f;

	if (output == nullptr)
		output = Matrix4x4::create();

	const float distance = std::max(CAMERA_MIN_DIST, std::min(CAMERA_MAX_DIST, cameraDistance));

	auto cameraPosition = Vector3::create(
			sinf(CAMERA_ANGLE_Y + rotation),
			sinf(CAMERA_ANGLE_X) * cosf(CAMERA_ANGLE_Y),
			cosf(CAMERA_ANGLE_X + rotation)
		)->scaleBy(distance);

	return output->lookAt(Vector3::zero(), cameraPosition);
}

void
explodeModel(float		magnitude,
			float		previousMagnitude,
			Node::Ptr	node)
{
	assert(fabsf(magnitude) > 1e-6f);
	assert(fabsf(previousMagnitude) > 1e-6f);

	for (auto& child : node->children())
	{
		if (child->component<Transform>())
		{
			auto transform	= child->component<Transform>()->matrix();
			auto direction	= transform->translation();

			transform->appendTranslation(direction * (magnitude - previousMagnitude));
			//transform->appendTranslation(direction * (magnitude / previousMagnitude));

			/*
			if (magnitude > 0.0f)
				transform->appendTranslation(direction * magnitude);
			else
				transform->appendTranslation((direction * magnitude) * (1.f /(magnitude - 1.f)) * -1);
				*/
			explodeModel(magnitude * 0.8f, previousMagnitude * 0.8f, child);
		}
		else
			explodeModel(magnitude, previousMagnitude, child);
	}
}
