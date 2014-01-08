#include <ctime>
#include <random>
#include <chrono>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoMk.hpp"

#include "Leap/Leap.h"

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::material;
using namespace minko::component;

void
controlWithLeap(const Leap::Controller&);

class SampleListener : public Leap::Listener
{
public:
	virtual void onInit(const Leap::Controller&);
	virtual void onConnect(const Leap::Controller&);
	virtual void onDisconnect(const Leap::Controller&);
	virtual void onExit(const Leap::Controller&);
	virtual void onFrame(const Leap::Controller&);
	virtual void onFocusGained(const Leap::Controller&);
	virtual void onFocusLost(const Leap::Controller&);
};

void 
SampleListener::onInit(const Leap::Controller& controller) 
{
	std::cout << "Initialized" << std::endl;
}

void 
SampleListener::onConnect(const Leap::Controller& controller) 
{
	std::cout << "Connected" << std::endl;
//	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
//	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void 
SampleListener::onDisconnect(const Leap::Controller& controller) 
{
	//Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void 
SampleListener::onExit(const Leap::Controller& controller) 
{
	std::cout << "Exited" << std::endl;
}

void 
SampleListener::onFrame(const Leap::Controller& controller) 
{
	controlWithLeap(controller);
	return;

	/***
	// Get the most recent frame and report some basic information
	const Leap::Frame frame = controller.frame();

	std::cout << "onFrame:\tconnected ?" << controller.isConnected() << std::endl;

	std::cout << "Frame id: " << frame.id()
		<< ", timestamp: " << frame.timestamp()
		<< ", hands: " << frame.hands().count()
		<< ", fingers: " << frame.fingers().count()
		<< ", tools: " << frame.tools().count()
		<< ", gestures: " << frame.gestures().count() << std::endl;

	if (!frame.hands().isEmpty()) 
	{
		// Get the first hand
		const Leap::Hand hand = frame.hands()[0];

		// Check if the hand has any fingers
		const Leap::FingerList fingers = hand.fingers();
		if (!fingers.isEmpty()) {
			// Calculate the hand's average finger tip position
			Leap::Vector avgPos;
			for (int i = 0; i < fingers.count(); ++i) 
			{
				avgPos += fingers[i].tipPosition();
			}
			avgPos /= (float)fingers.count();
			std::cout << "Hand has " << fingers.count()
				<< " fingers, average finger tip position" << avgPos << std::endl;
		}

		// Get the hand's sphere radius and palm position
		std::cout << "Hand sphere radius: " << hand.sphereRadius()
			<< " mm, palm position: " << hand.palmPosition() << std::endl;

		// Get the hand's normal vector and direction
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();

		// Calculate the hand's pitch, roll, and yaw angles
		std::cout << "Hand pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, "
			<< "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;
	}

	// Get gestures
	const Leap::GestureList gestures = frame.gestures();
	for (int g = 0; g < gestures.count(); ++g) 
	{
		Leap::Gesture gesture = gestures[g];

		switch (gesture.type()) 
		{
		case Leap::Gesture::TYPE_CIRCLE:
		{
										 Leap::CircleGesture circle = gesture;
										   std::string clockwiseness;

										   if (circle.pointable().direction().angleTo(circle.normal()) <= PI / 4) {
											   clockwiseness = "clockwise";
										   }
										   else {
											   clockwiseness = "counterclockwise";
										   }

										   // Calculate angle swept since last frame
										   float sweptAngle = 0;
										   if (circle.state() != Leap::Gesture::STATE_START) {
											   Leap::CircleGesture previousUpdate = Leap::CircleGesture(controller.frame(1).gesture(circle.id()));
											   sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
										   }
										   std::cout << "Circle id: " << gesture.id()
											   << ", state: " << gesture.state()
											   << ", progress: " << circle.progress()
											   << ", radius: " << circle.radius()
											   << ", angle " << sweptAngle * Leap::RAD_TO_DEG
											   << ", " << clockwiseness << std::endl;
										   break;
		}
		case Leap::Gesture::TYPE_SWIPE:
		{
										  Leap::SwipeGesture swipe = gesture;
										  std::cout << "Swipe id: " << gesture.id()
											  << ", state: " << gesture.state()
											  << ", direction: " << swipe.direction()
											  << ", speed: " << swipe.speed() << std::endl;
										  break;
		}
		case Leap::Gesture::TYPE_KEY_TAP:
		{
											Leap::KeyTapGesture tap = gesture;
											std::cout << "Key Tap id: " << gesture.id()
												<< ", state: " << gesture.state()
												<< ", position: " << tap.position()
												<< ", direction: " << tap.direction() << std::endl;
											break;
		}
		case Leap::Gesture::TYPE_SCREEN_TAP:
		{
											   Leap::ScreenTapGesture screentap = gesture;
											   std::cout << "Screen Tap id: " << gesture.id()
												   << ", state: " << gesture.state()
												   << ", position: " << screentap.position()
												   << ", direction: " << screentap.direction() << std::endl;
											   break;
		}
		default:
			std::cout << "Unknown gesture type." << std::endl;
			break;
		}
	}

	if (!frame.hands().isEmpty() || !gestures.isEmpty()) 
	{
		std::cout << std::endl;
	}
	***/
}

void SampleListener::onFocusGained(const Leap::Controller& controller) 
{
	std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Leap::Controller& controller) 
{
	std::cout << "Focus Lost" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

struct BoundingBox
{
	Node::Ptr		node;
	Vector3::Ptr	min;
	Vector3::Ptr	max;

	inline
	BoundingBox(Node::Ptr n, Vector3::Ptr minV, Vector3::Ptr maxV) :
		node(n), min(minV), max(maxV)
	{
	}
};


struct ModelExplosionInfo
{
	Node::Ptr	baseNode;
	Node::Ptr	camera;
	clock_t		start;
	bool		inProgress;
	bool		zoomed;
	int			fingerID;
	float		lastTicks;
	float		lastGap;
	float		currentExplodeValue;
	float		totalMoveTime;
	float		cameraDistance;
	float		angle;
	float		targetAngle;

	ModelExplosionInfo():
		baseNode(Node::create("baseNode")),
		camera(Node::create("camera")),
		start(clock()),
		inProgress(false),
		zoomed(false),
		fingerID(-1),
		lastTicks(0.0f),
		lastGap(0.0f),
		currentExplodeValue(0.0f),
		totalMoveTime(0.0f),
		cameraDistance(20.0f),
		angle(0.0f),
		targetAngle(0.0f)
	{
	}
};


static const std::string	MK_NAME			= "model/airplane_engine_n.mk";
static const std::string	DEFAULT_EFFECT	= "effect/Basic.effect";
static const float			CAMERA_ANGLE_X	= 35.0f * (float)PI / 180.0f;
static const float			CAMERA_ANGLE_Y	= 45.0f * (float)PI / 180.0f;

ModelExplosionInfo g_info; // FIXME: TEMP SOLUTION


void 
explode(scene::Node::Ptr node, float magnitude);

void
getBoundingBoxes(Node::Ptr node, std::vector<::BoundingBox>& boxList);

Node::Ptr 
RandomScene(scene::Node::Ptr base, int depth, float radius, file::AssetLibrary::Ptr assets);

Node::Ptr
getTouchedMesh(Node::Ptr camera, 
			   Node::Ptr pointer, 
			   std::vector<::BoundingBox>& boxList, 
			   Matrix4x4::Ptr worldToScreen);

int main(int argc, char** argv)
{
	// FIXME: need proper support of fullscreen
	bool fullscreen = false;
	if (argc >= 2 && strcmp(argv[1], "--fullscreen") == 0)
		fullscreen = true;

	int		windowWidth		= 800;
	int		windowHeight	= 640;
	auto	canvas			= Canvas::create("Minko Example - Leap", windowWidth, windowHeight);

	std::vector<::BoundingBox> boxList;
    
	/****
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		exit(-1);




#ifdef EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Minko - Cube Example", "Minko");
	SDL_Surface *screen = SDL_SetVideoMode(800,
		600,
		0, SDL_OPENGL);

	std::cout << "WebGL context created" << std::endl;
	//context = render::WebGLContext::create();
#else
	SDL_Window* window = SDL_CreateWindow(
		"Minko - Leap Motion Example",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 640,
		SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)
	);

# ifdef MINKO_ANGLE
	ESContext* context;
	if (!(context = initContext(window)))
		throw std::runtime_error("Could not create eglContext");

	std::cout << "EGLContext Initialized" << std::endl;
# else
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
# endif
#endif

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	***/

	// Leap Motion set-up
	SampleListener	leapListener;
	Leap::Controller leapController;
	leapController.addListener(leapListener);

	//leapController.enableGesture(Leap::Gesture::TYPE_SWIPE);
	//leapController.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);

	// scene set-up
	auto sceneManager		= SceneManager::create(render::OpenGLES2Context::create());

	auto ambientLightNode	= Node::create("ambientLight");
	auto pointer			= Node::create("pointer");
    auto skybox				= Node::create("skybox");
    auto pLight1			= Node::create("pointLight1");
    auto pLight2			= Node::create("pointLight2");
    auto root				= Node::create("root");
    
	auto ppRenderer			= Renderer::create();
	auto ppTarget			= render::Texture::create(sceneManager->assets()->context(), 2048, 2048, false, true);

	// setup assets
	auto defaultMaterial	= Material::create()
		->set("diffuseColor", Vector4::create(1.f, 1.f, 1.f, 1.f))
		->set("triangleCulling", render::TriangleCulling::NONE);

	sceneManager->assets()->context()->errorsEnabled(true);

	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()->defaultOptions()->material(std::static_pointer_cast<Material>(
			Material::create()->set("triangleCulling", render::TriangleCulling::NONE)
	));
	sceneManager->assets()->defaultOptions()->materialFunction([&](const std::string&, Material::Ptr)
	{
		return std::static_pointer_cast<Material>(defaultMaterial);
	});

	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context()))
		->geometry("skybox", geometry::SphereGeometry::create(sceneManager->assets()->context(), 80, 80, true))
        ->queue("texture/hangar.png");   
    
    sceneManager->assets()
		->load("effect/Phong.effect")
		->load("effect/Basic.effect")
		->load("effect/fxaa.effect");

	sceneManager->assets()->defaultOptions()->effect(sceneManager->assets()->effect("effect/Phong.effect"));
    
	sceneManager->assets()
		->queue(MK_NAME);
    
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		Node::Ptr mk = assets->symbol(MK_NAME);
		
		root->addComponent(sceneManager);

		auto lights = scene::Node::create();

		lights
			//->addComponent(component::AmbientLight::create())
			->addComponent(component::DirectionalLight::create())
			->addComponent(component::Transform::create());
		lights->component<Transform>()->matrix()->lookAt(Vector3::zero(), Vector3::create(-1.f, -1.f, -1.f));

		root->addChild(lights);

		// setup camera
		auto renderer = Renderer::create();
		renderer->backgroundColor(0x7F7F7FFF);

		g_info.camera
			->addComponent(renderer)
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(
				Vector3::zero(),
				Vector3::create(20.f * sinf(CAMERA_ANGLE_Y), 20.f * sinf(CAMERA_ANGLE_X) * cosf(CAMERA_ANGLE_X), 20.f * cosf(CAMERA_ANGLE_X))
			)))
			->addComponent(PerspectiveCamera::create(windowWidth / (float)windowHeight));

		root->addChild(g_info.camera);

		g_info.baseNode->addComponent(Transform::create());

		//RandomScene(baseNode, 3, 2.f, assets);
		g_info.baseNode->addChild(mk);

		pointer->addComponent(Transform::create());
		pointer->component<Transform>()->matrix()->prependScale(0.3f, 0.3f, 0.3f)->appendTranslation(0, 0, -5.0f);
		pointer->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f)),
			assets->effect("effect/Phong.effect")
		));

        pLight1->addComponent(component::PointLight::create());
        pLight1->addComponent(component::Transform::create());
        pLight1->component<Transform>()->matrix()->appendTranslation(0.f, 10.f, 0.f);
        pLight1->component<PointLight>()->color()->setTo(1.f, 1.f, 1.f);
        
        skybox->addComponent(Transform::create());
        skybox->component<Transform>()->matrix()->prependScale(60.0f, 60.0f, 60.0f);
        skybox->addComponent(Surface::create(
             assets->geometry("skybox"),
             data::Provider::create()
             
             ->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 1.f))
             ->set("material.diffuseMap",	assets->texture("texture/hangar.png"))
             ->set("material.triangleCulling", render::TriangleCulling::FRONT),
             assets->effect("effect/Basic.effect")
                             ));
        
        root->addChild(skybox);
		root->addChild(g_info.baseNode);
        root->addChild(pLight1);
        
		/*/
		// post-processing
		auto ppFx = sceneManager->assets()->effect("effect/fxaa.effect");

		if (!ppFx)
			throw std::logic_error("fxaa.effect has not been loaded.");

		ppTarget->upload();

		sceneManager->assets()->context()->errorsEnabled(true);
		auto ppScene = Node::create()
			->addComponent(ppRenderer)
			->addComponent(Surface::create(
				geometry::QuadGeometry::create(sceneManager->assets()->context()),
				data::Provider::create()
					->set("backbuffer", ppTarget)
					->set("texcoordOffset", Vector2::create(1.0f/2048.f, 1.0f/2048.0f)),
				ppFx
			));
			*/
        //getBoundingBoxes(baseNode, *boxList);

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, uint deltaTime)
		{
			//controlWithLeap(leapController, camera);

			sceneManager->nextFrame();
		});

		canvas->run();
	});
    
	sceneManager->assets()->load();

	return 0;
    
	/**
    PerspectiveCamera::Ptr cam = camera->component<PerspectiveCamera>();
    
	Leap::Controller* controller = new Leap::Controller();
	controller->enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller->enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    
    
	Leap::Frame lastFrame;
	float angle = 0.f;
	float targetAngle = 0.f;
	float cameraDistance = 20.f;
	float targetcameraDistance = 20.f;
	bool inProgress = false;
	float totalMoveTime = 0;
	float explodeThreshold = 1.f;
	bool exploded = false;
	float currentExplodeValue = 0.f;
	bool zoomed = false;

    int fingerID = -1;
	Uint32 lastTicks = 0;
	float frameTime;
    
	Vector3::Ptr targetPos = Vector3::create();
	float g_info.lastGap = 0.0f;
	const float delta = 5.f;
    
	bool done = false;
	while (!done)
	{
		SDL_Event event;

		Uint32 ticks = SDL_GetTicks();
		frameTime = (ticks - lastTicks) / 1000.f;
		lastTicks = ticks;
        
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
                case SDL_QUIT:
                    done = true;
                    break;
                default:
                    break;
			}
		}

		const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
		if (keyboardState[SDL_SCANCODE_ESCAPE])
			done = true;
        
		Leap::Frame frame = controller->frame();
        
		const Leap::GestureList gestures = frame.gestures();
        
		const Leap::HandList hands = frame.hands();
        
		
        
		if (hands.count() >= 2)
		{
			Leap::Hand lhand = hands.leftmost();
			Leap::Hand rhand = hands.rightmost();
            
			float gap = (rhand.palmPosition() - lhand.palmPosition()).magnitude();;
            if (g_info.lastGap < 0)
			{
				g_info.lastGap = gap;
			}
			if (gap > g_info.lastGap + delta)
			{
				totalMoveTime += 10 * frameTime;
				//scaleSpeed = scaleSpeed + (1.5f - scaleSpeed) * 0.01f;
			}
			else if (gap < g_info.lastGap - delta)
			{
				totalMoveTime -= 10 * frameTime;
				//scaleSpeed = scaleSpeed + (0.5f - scaleSpeed) * 0.01f;
			}

			if(!inProgress)
			if (totalMoveTime >= explodeThreshold)
			{
				inProgress = true;
				exploded = true;
				totalMoveTime = 0;
				std::cout << "explode" << std::endl;
			}
			else if (totalMoveTime <= -explodeThreshold)
			{
				inProgress = true;
				exploded = false;
				totalMoveTime = 0;
				std::cout << "unexplode" << std::endl;
			}
			g_info.lastGap = gap;
            
			totalMoveTime -= 0.5 * frameTime * ((0 < totalMoveTime) - (0 > totalMoveTime));
		}
		else
		{
			g_info.lastGap = -1;
			for (int g = 0; g < gestures.count(); g++)
			{
				Leap::Gesture gesture = gestures[g];
				switch (gesture.type())
				{
                    case Leap::Gesture::TYPE_SWIPE:
					{
						Leap::SwipeGesture swipe = gesture;
						if (fabs((swipe.direction().x)) > 0.5f)
						{
							if (gesture.state() == Leap::Gesture::State::STATE_START && !inProgress)
							{
								inProgress = true;
								if (fabs((angle - targetAngle)) < PI / 10.f);
								targetAngle += PI / 2.f * ((0.f < swipe.direction().x) - (swipe.direction().x < 0.f));
							}
						}
						else
						{
							if (fabs(swipe.direction().y) > 0.7)
								zoomed = swipe.direction().y < 0;
						}
						break;
					}
                    case Leap::Gesture::TYPE_SCREEN_TAP:
					{
						//pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
                        //Leap::ScreenTapGesture screenTape = gesture;
                        
                        //std::cout << screenTape.direction().x << ":"<< screenTape.direction().y <<":"<< screenTape.direction().z << std::endl;
                        //std::cout << screenTape.position().x << ":"<< screenTape.position().y <<":"<< screenTape.position().z << std::endl;
                        
                        //auto touche = getTouchedMesh(camera,
                         //                            pointer,
                          //                           *boxList,
                           //                          worldToScreen);
                        //if (touche)
                          //  touche->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
                        break;
					}
                    default:
                        break;
				}
				
			}

			Leap::PointableList	pointables	= frame.pointables();
			Leap::Pointable		finger		= frame.finger(fingerID);
			if (!finger.isValid())
			{
				if (pointables.count() >= 1)
				{
					finger = pointables[0];
					fingerID = finger.id();
				}
			}
			if (finger.isValid())
			{
				auto tip = finger.tipPosition() * 0.05f;
				targetPos->lerp(Vector3::create(tip.x, tip.y, -15.f), frameTime * 2);
			}
			else
				fingerID = -1;
		}

		float explodeTarget = exploded ? 1.5f : 0.f;
        if (fabs((targetAngle - angle)) > PI / 2 * 0.01f)
		{
			angle = angle + (targetAngle - angle) * frameTime * 2;
		}
		
		if (fabs((targetAngle - angle)) < PI / 2 * 0.1f
			&& fabs((explodeTarget - currentExplodeValue)) < 0.05f)
				inProgress = false;

		
		float explodeDelta = currentExplodeValue + (explodeTarget - currentExplodeValue) * frameTime * 2;
		explode(baseNode, explodeDelta - currentExplodeValue);
		currentExplodeValue = explodeDelta;

		targetcameraDistance = zoomed ? 10.f : 20.f;
		cameraDistance = cameraDistance + (targetcameraDistance - cameraDistance) * frameTime * 2;
        camera->component<Transform>()->matrix()->identity()
			->lookAt(Vector3::zero(), Vector3::create(cameraDistance * sin(CAMERA_ANGLE_Y + angle),
													  cameraDistance * sin(CAMERA_ANGLE_X) * cos(CAMERA_ANGLE_Y),
													  cameraDistance * cos(CAMERA_ANGLE_X + angle)));
		
		//pointer->component<Transform>()->matrix()->identity()->appendScale(0.1f, 0.1f, 0.1f)->prependTranslation(targetPos);
		//scaleSpeed = scaleSpeed + (1.f - scaleSpeed) * 0.1f;
		//std::cout << scaleSpeed << std::endl;
        
		sceneManager->nextFrame(ppTarget);
		ppRenderer->render(sceneManager->assets()->context());
        
		//pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f));
#ifdef MINKO_ANGLE
		eglSwapBuffers(context->eglDisplay, context->eglSurface);
#elif defined(EMSCRIPTEN)
		SDL_GL_SwapBuffers();
#else
		SDL_GL_SwapWindow(window);
#endif
	}
    
	SDL_Quit();
    
	exit(EXIT_SUCCESS);
	*****/
}


void
controlWithLeap(const Leap::Controller& controller)
{
	if (!controller.isConnected())
		return;

	const float	delta = 5.0f;
	const float explodeThreshold = 1.f;

	const float ticks		= 1000.0f * (float)( clock() - g_info.start ) / (float)CLOCKS_PER_SEC; // in milliseconds
	const float	frameTime	= (ticks - g_info.lastTicks) * 0.001f;
	g_info.lastTicks		= ticks;

	const Leap::Frame&			frame = controller.frame();
	const Leap::GestureList&	gestures = frame.gestures();
	const Leap::HandList&		hands = frame.hands();

	bool exploded = false;
	auto targetPos = Vector3::create();

	if (hands.count() >= 2)
	{
		const Leap::Hand& lhand = hands.leftmost();
		const Leap::Hand& rhand = hands.rightmost();

		float gap = ( rhand.palmPosition() - lhand.palmPosition() ).magnitude();
		if (g_info.lastGap < 0.0f)
		{
			g_info.lastGap = gap;
		}
		if (gap > g_info.lastGap + delta)
		{
			g_info.totalMoveTime += 10 * frameTime;
			//scaleSpeed = scaleSpeed + (1.5f - scaleSpeed) * 0.01f;
		}
		else if (gap < g_info.lastGap - delta)
		{
			g_info.totalMoveTime -= 10 * frameTime;
			//scaleSpeed = scaleSpeed + (0.5f - scaleSpeed) * 0.01f;
		}

		if (!g_info.inProgress)
			if (g_info.totalMoveTime >= explodeThreshold)
			{
				g_info.inProgress = true;
				exploded = true;
				g_info.totalMoveTime = 0;
				std::cout << "explode" << std::endl;
			}
			else if (g_info.totalMoveTime <= -explodeThreshold)
			{
				g_info.inProgress = true;
				exploded = false;
				g_info.totalMoveTime = 0;
				std::cout << "unexplode" << std::endl;
			}
		g_info.lastGap = gap;

		g_info.totalMoveTime -= 0.5f * frameTime * ( ( 0.0f < g_info.totalMoveTime ? 1.0f : 0.0f ) - ( 0.0f > g_info.totalMoveTime ? 1.0f : 0.0f ) );
	}
	else
	{
		g_info.lastGap = -1;
		for (int g = 0; g < gestures.count(); g++)
		{
			Leap::Gesture gesture = gestures[g];
			switch (gesture.type())
			{
			case Leap::Gesture::TYPE_SWIPE:
			{
											  Leap::SwipeGesture swipe = gesture;
											  if (fabs(( swipe.direction().x )) > 0.5f)
											  {
												  if (gesture.state() == Leap::Gesture::State::STATE_START && !g_info.inProgress)
												  {
													  g_info.inProgress = true;
													  if (fabs(( g_info.angle - g_info.targetAngle )) < PI / 10.f)
														g_info.targetAngle += (float)PI / 2.f * ( ( 0.f < swipe.direction().x ? 1.0f : 0.0f) - ( swipe.direction().x < 0.f ? 1.0f : 0.0f) );
												  }
											  }
											  else
											  {
												  if (fabs(swipe.direction().y) > 0.7)
													  g_info.zoomed = swipe.direction().y < 0.f;
											  }
											  break;
			}
			case Leap::Gesture::TYPE_SCREEN_TAP:
			{
												   //pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
												   //Leap::ScreenTapGesture screenTape = gesture;

												   //std::cout << screenTape.direction().x << ":"<< screenTape.direction().y <<":"<< screenTape.direction().z << std::endl;
												   //std::cout << screenTape.position().x << ":"<< screenTape.position().y <<":"<< screenTape.position().z << std::endl;

												   //auto touche = getTouchedMesh(camera,
												   //                            pointer,
												   //                           *boxList,
												   //                          worldToScreen);
												   //if (touche)
												   //  touche->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
												   break;
			}
			default:
				break;
			}

		}

		Leap::PointableList	pointables = frame.pointables();
		Leap::Pointable		finger = frame.finger(g_info.fingerID);
		if (!finger.isValid())
		{
			if (pointables.count() >= 1)
			{
				finger = pointables[0];
				g_info.fingerID = finger.id();
			}
		}
		if (finger.isValid())
		{
			auto tip = finger.tipPosition() * 0.05f;
			targetPos->lerp(Vector3::create(tip.x, tip.y, -15.f), frameTime * 2);
		}
		else
			g_info.fingerID = -1;
	}

	float explodeTarget = exploded ? 1.5f : 0.f;
	if (fabs(( g_info.targetAngle - g_info.angle )) > PI / 2 * 0.01f)
	{
		g_info.angle = g_info.angle + ( g_info.targetAngle - g_info.angle ) * frameTime * 2;
	}

	if (fabs(( g_info.targetAngle - g_info.angle )) < PI / 2 * 0.1f
		&& fabs(( explodeTarget - g_info.currentExplodeValue )) < 0.05f)
		g_info.inProgress = false;


	float explodeDelta = g_info.currentExplodeValue + ( explodeTarget - g_info.currentExplodeValue ) * frameTime * 2;
	explode(g_info.baseNode, explodeDelta - g_info.currentExplodeValue);
	g_info.currentExplodeValue = explodeDelta;

	const float targetcameraDistance = g_info.zoomed ? 10.f : 20.f;

	g_info.cameraDistance = g_info.cameraDistance + ( targetcameraDistance - g_info.cameraDistance ) * frameTime * 2;

	if (g_info.camera->hasComponent<Transform>())
		g_info.camera->component<Transform>()->matrix()->identity()
			->lookAt(
				Vector3::zero(), 
				Vector3::create(
					g_info.cameraDistance * sin(CAMERA_ANGLE_Y + g_info.angle),
					g_info.cameraDistance * sin(CAMERA_ANGLE_X) * cos(CAMERA_ANGLE_Y),
					g_info.cameraDistance * cos(CAMERA_ANGLE_X + g_info.angle
				)
			)
		);
}

void 
explode(scene::Node::Ptr node, 
		float magnitude)
{
	for (auto& child : node->children())
	{
		if (child->component<Transform>())
		{
			auto cTransform	= child->component<Transform>()->matrix();
			auto direction	= cTransform->translation();
			if (magnitude > 0.0f)
				cTransform->appendTranslation(direction * magnitude);
			else
				cTransform->appendTranslation((direction * magnitude) * (1.f /(magnitude - 1.f)) * -1);
			explode(child, magnitude * 0.8f);
		}
		else
			explode(child, magnitude);
	}
}

void
getBoundingBoxes(Node::Ptr node, 
				 std::vector<::BoundingBox>& boxList)
{
    auto surface = node->component<Surface>();

    if (surface != nullptr)
    {
        auto	geometry	= surface->geometry();
        auto	provider	= geometry->data();
        auto	vertices	= provider->get<render::VertexBuffer::Ptr>("geometry.vertex.attribute.position")->data();
        auto	max			= Vector3::create(vertices[0], vertices[1], vertices[2]);
        auto	min			= Vector3::create(vertices[0], vertices[1], vertices[2]);
        uint	vertexSize	= provider->get<render::VertexBuffer::Ptr>("geometry.vertex.attribute.position")->vertexSize();

        for (unsigned int i = 0; i < vertices.size(); i +=vertexSize)
        {
            if (vertices[i] > max->x())
                max->x(vertices[i]);
            if (vertices[i+1] > max->y())
                max->y(vertices[i+1]);
            if (vertices[i+2] > max->z())
                max->z(vertices[i+2]);
            
            if (vertices[i] < min->x())
                min->x(vertices[i]);
            if (vertices[i+1] < min->y())
                min->y(vertices[i+1]);
            if (vertices[i+2] < min->z())
                min->z(vertices[i+2]);
        }

		boxList.push_back(::BoundingBox(node, min, max));
    }

    for (auto& child : node->children())
        getBoundingBoxes(child, boxList);
}


Node::Ptr 
RandomScene(scene::Node::Ptr base, int depth, float radius, file::AssetLibrary::Ptr assets)
{
	if (depth == 0)
		return base;
	std::random_device rd;
	std::default_random_engine generator (rd());
	std::uniform_real_distribution<float> distribution(0.f, 1.f);
	auto randf = std::bind (distribution, generator);

	int numObjects = rand() % 6;
	for (int i = 0; i < numObjects; i++)
	{
		auto node = scene::Node::create();

		int type = rand() % 2;
		if (type == 0)
			node->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
			->set("material.diffuseColor",	Vector4::create(randf(), randf(), randf(), 1.f)),
			assets->effect("effect/Basic.effect")
			));
		else
			node->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor",	Vector4::create(randf(), randf(), randf(), 1.f)),
			assets->effect("effect/Basic.effect")
			));

		node->addComponent(Transform::create());
		auto position = Vector3::create(randf() * 2.f - 1.f, randf() * 2.f - 1.f, randf() * 2.f - 1.f);
		position = position * radius;
		node->component<Transform>()->matrix()->appendTranslation(position)->prependScale(depth / 2.f, depth / 2.f, depth / 2.f);
		base->addChild(node);
		RandomScene(node, depth - 1, radius / 2, assets);
		}
	return base;
}


Node::Ptr
getTouchedMesh(Node::Ptr camera, 
			   Node::Ptr pointer, 
			   std::vector<::BoundingBox>& boxList, 
			   Matrix4x4::Ptr worldToScreen)
{
    Node::Ptr result;
    std::vector<Node::Ptr> touchedNodes;
    
    for (auto& box : boxList)
    {
        auto nodeTransform = box.node->component<Transform>();
        auto modelToWorldMat = nodeTransform->modelToWorldMatrix();
        auto worldToModel = modelToWorldMat->invert();
        
        auto modelCameraTransform = Matrix4x4::create(camera->component<Transform>()->matrix());
        modelCameraTransform->append(worldToModel);
        
        auto modelPointerTransform = Matrix4x4::create(pointer->component<Transform>()->matrix());
        modelPointerTransform->append(worldToModel);
        
        auto rayOriginTransform = Matrix4x4::create(pointer->component<Transform>()->matrix());
        rayOriginTransform->appendTranslation(0.f, 0.f, 20.f);
        rayOriginTransform->append(worldToModel);
        
        auto rayStart = modelCameraTransform->translation();
        auto pointerTranslation =modelPointerTransform->translation();
        
        auto direction = (modelPointerTransform->translation() - modelCameraTransform->translation())->normalize();
        
        float t0x, t0y, t0z,t1x, t1y, t1z;
        auto max = box.max;
        auto min = box.min;
        
        Vector3::Ptr nearCorner, farCorner;
        if (max->z() > min->z())
        {
            nearCorner = max;
            farCorner = min;
        }
        else
        {
            nearCorner = min;
            farCorner = max;
        }
        float tmin, tmax;
        t0x = (nearCorner->x() - rayStart->x()) / direction->x();
        t1x = (farCorner->x() - rayStart->x()) / direction->x();
        if (t0x > t1x) std::swap(t0x, t1x);
        tmin = t0x;
        tmax = t1x;
        
        t0y = (nearCorner->y() - rayStart->y()) / direction->y();
        t1y = (farCorner->y() - rayStart->y()) / direction->y();
        if (t0y > t1y) std::swap(t0y, t1y);
        if (t0y > tmax || tmin > t1y)
            continue;
        if (t0y > tmin)
            tmin = t0y;
        if (t1y < tmax)
            tmax = t1y;
        
        t0z = (nearCorner->z() - rayStart->z()) / direction->z();
        t1z = (farCorner->z() - rayStart->z()) / direction->z();
        if (t0z > t1z) std::swap(t0z, t1z);
        if (t0z > tmax || tmin > t1z)
            continue;
        if (t0z > tmin)
            tmin = t0z;
        if (t1z < tmax)
            tmax = t1z;
 
        //result = box.node;
        touchedNodes.push_back(box.node);
    }
    
//    for (BoundingBox box : boxList)
//    {
//        auto nodeTransform = Matrix4x4::create(box.node->component<Transform>()->matrix());
//        auto modelToWorldMat = box.node->component<Transform>()->modelToWorldMatrix();
//        auto modelPointerTransform = Matrix4x4::create(pointer->component<Transform>()->matrix());
//        
//        nodeTransform->append(modelToWorldMat);
//        nodeTransform->append(worldToScreen);
//        modelPointerTransform->append(pointer->component<Transform>()->modelToWorldMatrix());
//        modelPointerTransform->append(worldToScreen);
//        
//        auto max = nodeTransform->transform(box.max);
//        auto min = nodeTransform->transform(box.min);
//    
//        Vector3::Ptr nearCorner, farCorner;
//        if (max->x() > min->x())
//        {
//            nearCorner = max;
//            farCorner = min;
//        }
//        else
//        {
//            nearCorner = min;
//            farCorner = max;
//        }
//
//        if (modelPointerTransform->translation()->x() > farCorner->x()
//            && modelPointerTransform->translation()->x() < nearCorner->x())
//        {
//            if (modelPointerTransform->translation()->y() > farCorner->y()
//                && modelPointerTransform->translation()->y() < nearCorner->y())
//            {
//                touchedNodes.push_back(box.node);
//                result = box.node;
//            }
//        }
//    }
    
    //Get only the nearest mesh
    if (touchedNodes.size() > 0)
        result = touchedNodes[0];
    
    for (unsigned int i = 0; i < touchedNodes.size(); i++)
    {
        auto touchedNodeTransform = Matrix4x4::create(touchedNodes[i]->component<Transform>()->matrix());
        auto resultNodeTransform = Matrix4x4::create(result->component<Transform>()->matrix());

        auto touchedNodeZ = touchedNodeTransform->append(touchedNodes[i]->component<Transform>()->modelToWorldMatrix())->translation()->z();
        auto resultNodetZ = resultNodeTransform->append(result->component<Transform>()->modelToWorldMatrix())->translation()->z();
        
        if (touchedNodeZ > resultNodetZ)
            result = touchedNodes[i];
    }
    return result;
}