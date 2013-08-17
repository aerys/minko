#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoMk.hpp"
#include "minko/MinkoBullet.hpp"
#include "minko/MinkoParticles.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"

#include <time.h>

#ifdef EMSCRIPTEN
#include "minko/MinkoWebGL.hpp"
#include "GL/glut.h"
#include "emscripten.h"
#else
#include "GLFW/glfw3.h"
#endif

#include "minko/component/SponzaLighting.hpp"
#include "minko/component/Fire.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const float WINDOW_WIDTH		= 1024.0f;
const float WINDOW_HEIGHT		= 500.0f;

const std::string MK_NAME			= "model/Sponza_lite_sphere.mk";
const std::string DEFAULT_EFFECT	= "effect/SponzaLighting.effect";
const std::string CAMERA_NAME		= "camera";

const float CAMERA_LIN_SPEED	= 0.05f;
const float CAMERA_ANG_SPEED	= PI * 2.f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;

Renderer::Ptr			renderer			= nullptr;
auto					sponzaLighting		= SponzaLighting::create();
auto					mesh				= scene::Node::create("mesh");
auto					group				= scene::Node::create("group");
auto					camera				= scene::Node::create("camera");
auto					root				= scene::Node::create("root");
auto					speed				= 0.0f;
auto					angSpeed			= 0.0f;
float					rotationX			= 0.0f;
float					rotationY			= 0.0f;
float					mousePositionX		= 0.0f;
float					mousePositionY		= 0.0f;
Vector3::Ptr			target				= Vector3::create();
Vector3::Ptr			eye					= Vector3::create();
bullet::Collider::Ptr	cameraCollider		= nullptr;

#if defined EMSCRIPTEN
render::WebGLContext::Ptr       context;
#else
render::OpenGLES2Context::Ptr   context;
#endif

#if defined EMSCRIPTEN
void
resizeHandler(int width, int height)
{
	context->configureViewport(0, 0, width, height);
}

void
keyDownHandler(int key, int x, int y)
{
	std::cout << "keyDownHandler: " << key << std::endl;
	if (cameraCollider == nullptr)
	{
		if (key == GLUT_KEY_UP)
			camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, -CAMERA_LIN_SPEED);
		else if (key == GLUT_KEY_DOWN)
			camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, CAMERA_LIN_SPEED);
		if (key == GLUT_KEY_LEFT)
			camera->component<Transform>()->transform()->prependRotation(-CAMERA_ANG_SPEED, Vector3::yAxis());
		else if (key == GLUT_KEY_RIGHT)
			camera->component<Transform>()->transform()->prependRotation(CAMERA_ANG_SPEED, Vector3::yAxis());
	}
	else
	{
		if (key == GLUT_KEY_UP)
			speed = -CAMERA_LIN_SPEED;
		else if (key == GLUT_KEY_DOWN)
			speed = CAMERA_LIN_SPEED;
		if (key == GLUT_KEY_LEFT)
			angSpeed = CAMERA_ANG_SPEED;
		else if (key == GLUT_KEY_RIGHT)
			angSpeed = -CAMERA_ANG_SPEED;
	}
}

void
keyUpHandler(int key, int x, int y)
{
	if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
		speed = 0;
	if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT)
		angSpeed = 0;
}

void
glutMouseMoveHandler(int x, int y)
{
	rotationY += -(mousePositionX - x) * .005;
	rotationX +=  (mousePositionY - y) * .005;

	const float limit = 89 * PI / 180;

	if (rotationX < -limit)
		rotationX = -limit;
	else if (rotationX > +limit)
		rotationX = +limit;

	mousePositionX = x;
	mousePositionY = y;
}

void
renderScene()
{
	auto cameraTransform = camera->component<Transform>()->transform();
	if (cameraCollider == nullptr)
	{
		if (speed)
			cameraTransform->prependTranslation(0.f, 0.f, speed);
		if (angSpeed)
		    cameraTransform->prependRotationY(angSpeed);
	}
	else
	{
		// the camera has a collider component

		// move forward/backward
		if (speed)
			cameraTransform->prependTranslation(0.0f, 0.0f, speed);

		// look around
		eye = cameraTransform->translationVector();

		target->setTo(
			eye->x() + sinf(rotationY) * cosf(rotationX),
			eye->y() + sinf(rotationX),
			eye->z() + cosf(rotationY) * cosf(rotationX)
		);
		
		cameraTransform->view(eye, target, Vector3::upAxis());
		
		auto newEyePos = cameraTransform->translationVector();
		
		cameraTransform->appendTranslation(
			eye->x() - newEyePos->x(),
			eye->y() - newEyePos->y(),
			eye->z() - newEyePos->z()
		);
		
		cameraCollider->synchronizePhysicsWithGraphics();
	}

	sponzaLighting->step();
	renderer->render();

	glutSwapBuffers();
}
#else
void
glfwMouseMoveHandler(GLFWwindow* window, double x, double y)
{
	rotationY += (mousePositionX - x) * .005;
	rotationX += (mousePositionY - y) * .005;

	const float limit = 89 * PI / 180;

	if (rotationX < -limit)
		rotationX = -limit;
	else if (rotationX > +limit)
		rotationX = +limit;

	mousePositionX = x;
	mousePositionY = y;
}
#endif

template <typename T>
static
void
read(std::stringstream& stream, T& value)
{
	stream.read(reinterpret_cast<char*>(&value), sizeof (T));
}

template <typename T>
static
T
swap_endian(T u)
{
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = u;

	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}

template <typename T>
T
readAndSwap(std::stringstream& stream)
{
	T value;
	stream.read(reinterpret_cast<char*>(&value), sizeof (T));

	return swap_endian(value);
}

bullet::AbstractPhysicsShape::Ptr
deserializeShape(Qark::Map&			shapeData,
				 scene::Node::Ptr&	node)
{
	bullet::AbstractPhysicsShape::Ptr deserializedShape;

	int type = Any::cast<int>(shapeData["type"]);

	double rx	= 0.0;
	double ry	= 0.0;
	double rz	= 0.0;
	double h	= 0.0;
	double r	= 0.0;

	std::stringstream stream;
	switch (type)
	{
	case 101: // multiprofile
		deserializedShape = deserializeShape(Any::cast<Qark::Map&>(shapeData["shape"]), node);
		break;
	case 2: // BOX
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			rx = readAndSwap<double>(stream);
			ry = readAndSwap<double>(stream);
			rz = readAndSwap<double>(stream);

			deserializedShape = bullet::BoxShape::create(rx, ry, rz);
		}
		break;
	case 5 : // CONE
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			r = readAndSwap<double>(stream);
			h = readAndSwap<double>(stream);

			deserializedShape = bullet::ConeShape::create(r, h);
		}
		break;
	case 6 : // BALL
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			r = readAndSwap<double>(stream);

			deserializedShape = bullet::SphereShape::create(r);
		}
		break;
	case 7 : // CYLINDER
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			r = readAndSwap<double>(stream);
			h = readAndSwap<double>(stream);

			deserializedShape = bullet::CylinderShape::create(r, h, r);
		}
		break;
	case 100 : // TRANSFORM
		{
			deserializedShape		= deserializeShape(Any::cast<Qark::Map&>(shapeData["subGeometry"]), node);

			auto delta				= deserialize::TypeDeserializer::matrix4x4(shapeData["delta"]);
			auto modelToWorld		= node->component<Transform>()->modelToWorldMatrix(true);

#ifdef DEBUG
			std::cout << "[" << node->name() << "]\tdeserialize TRANSFORM" << std::endl;

			component::bullet::PhysicsWorld::print(std::cout << "- delta = \n", delta) << std::endl;
			component::bullet::PhysicsWorld::print(std::cout << "- world = \n", modelToWorld) << std::endl;
#endif // DEBUG

			deserializedShape->initialize(delta, modelToWorld);
		}
		break;
	default:
		deserializedShape = nullptr;
	}

	return deserializedShape;		
}

std::shared_ptr<bullet::Collider>
deserializeBullet(Qark::Map&						nodeInformation, 
				  file::MkParser::ControllerMap&	controllerMap,
				  file::MkParser::NodeMap&			nodeMap,
				  scene::Node::Ptr&					node)
{
	Qark::Map& colliderData = Any::cast<Qark::Map&>(nodeInformation["defaultCollider"]);
	Qark::Map& shapeData	= Any::cast<Qark::Map&>(colliderData["shape"]);

	bullet::AbstractPhysicsShape::Ptr shape = deserializeShape(shapeData, node);

	float mass			= 1.0f;
	double vx			= 0.0;
	double vy			= 0.0;
	double vz			= 0.0;
	double avx			= 0.0;
	double avy			= 0.0;
	double avz			= 0.0;
	bool sleep			= false;
	bool rotate			= false;
	bool trigger		= false;
	double friction		= 0.5; // bullet's advices
	double restitution	= 0.0; // bullet's advices

	if (shapeData.find("materialProfile") != shapeData.end())
	{
		Qark::ByteArray& materialProfileData = Any::cast<Qark::ByteArray&>(shapeData["materialProfile"]);
		std::stringstream	stream;
		stream.write(&*materialProfileData.begin(), materialProfileData.size());

		double density	= readAndSwap<double>(stream);
		mass			= density * shape->volume();
		friction		= readAndSwap<double>(stream);
		restitution		= readAndSwap<double>(stream);
	}

	if (shapeData.find("logicProfile") != shapeData.end())
	{
		Qark::ByteArray& logicProfileData = Any::cast<Qark::ByteArray&>(shapeData["logicProfile"]);
		std::stringstream	stream;
		stream.write(&*logicProfileData.begin(), logicProfileData.size());

		trigger	= readAndSwap<bool>(stream);
	}

	if (colliderData.find("dynamics") == colliderData.end())
		mass = 0.0; // static object
	else
	{
		Qark::ByteArray& dynamicsData = Any::cast<Qark::ByteArray&>(colliderData["dynamics"]);
		std::stringstream	stream;
		stream.write(&*dynamicsData.begin(), dynamicsData.size());

		vx		= readAndSwap<double>(stream);
		vy		= readAndSwap<double>(stream);
		vz		= readAndSwap<double>(stream);
		avx		= readAndSwap<double>(stream);
		avy		= readAndSwap<double>(stream);
		avz		= readAndSwap<double>(stream);
		sleep	= readAndSwap<bool>(stream);
		rotate	= readAndSwap<bool>(stream);
	}


	bullet::ColliderData::Ptr data = bullet::ColliderData::create(mass, shape);

	data->linearVelocity(vx, vy, vz);
	data->angularVelocity(avx, avy, avz);
	data->friction(friction);
	data->restitution(restitution);
	data->triggerCollisions(trigger);

	if (!rotate)
		data->angularFactor(0.0f, 0.0f, 0.0f);
	//collider->disableDeactivation(sleep == false);
	data->disableDeactivation(true);

	return bullet::Collider::create(data);
}

component::bullet::Collider::Ptr
initializeDefaultCameraCollider()
{
	auto shape		= bullet::BoxShape::create(0.2f, 0.3f, 0.2f);
	auto data		= bullet::ColliderData::create(CAMERA_MASS, shape);

	data->restitution(0.5f);
	data->angularFactor(0.0f, 0.0f, 0.0f);
	data->friction(CAMERA_FRICTION);
	data->disableDeactivation(true);
	
	return bullet::Collider::create(data);
}

void
initializeCamera(scene::Node::Ptr group)
{
	auto cameras = scene::NodeSet::create(group)
		->descendants(true)
		->where([](scene::Node::Ptr node)
				{
					return node->name() == CAMERA_NAME;
				});
	
	bool cameraInGroup = false;
	if (cameras->nodes().empty())
	{
		std::cout << "MANUAL CAMERA" << std::endl;

		// default camera
		camera = scene::Node::create(CAMERA_NAME);

		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->appendTranslation(0.0f, 0.75f, 5.0f)
			->appendRotationY(PI * 0.5);
		
		cameraCollider = initializeDefaultCameraCollider();
		camera->addComponent(cameraCollider);
	}
	else
	{
		// set-up camera from the mk file
		camera = cameras->nodes().front();
		cameraInGroup = true;

		if (camera->hasComponent<component::bullet::Collider>())
		{
			std::cout << "PARSED CAMERA & COLLIDER" << std::endl;
			cameraCollider = camera->component<component::bullet::Collider>();
		}
		else
		{
			std::cout << "PARSED CAMERA W/OUT COLLIDER" << std::endl;
		}
	}

	if (!camera->hasComponent<Transform>())
		throw std::logic_error("Camera (deserialized or created) must have a Transform.");

	camera->addComponent(renderer);
	camera->addComponent(PerspectiveCamera::create(.785f, WINDOW_WIDTH / WINDOW_HEIGHT, .1f, 1000.f));

	root->addChild(camera);
}

void
initializePhysics()
{
	auto physicWorld = bullet::PhysicsWorld::create(renderer);

	physicWorld->setGravity(math::Vector3::create(0.f, -9.8f, 0.f));
	root->addComponent(physicWorld);
}


void
printFramerate(const unsigned int delay = 1)
{
	static auto start = time(NULL);
	static auto numFrames = 0;

	int secondTime = time(NULL);

	++numFrames;

	if ((secondTime - start) >= 1)
	{
		std::cout << numFrames << " fps." << std::endl;
		start = time(NULL);
		numFrames = 0;
	}
}

int
main(int argc, char** argv)
{
	file::MkParser::registerController(
		"colliderController",
		std::bind(
			deserializeBullet,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4
		)
	);

#ifdef EMSCRIPTEN
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Sponza Example");
	glutReshapeFunc(resizeHandler);

	std::cout << "WebGL context created" << std::endl;
	context = render::WebGLContext::create();
#else
	glfwInit();
	auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sponza Example", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, glfwMouseMoveHandler);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::cout << "OpenGL ES2 context created" << std::endl;
	context = render::OpenGLES2Context::create();
#endif
	
	std::cout << context->driverInfo() << std::endl;
	
	auto sceneManager = SceneManager::create(context);
	
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context));
	
#ifdef EMSCRIPTEN
	sceneManager->assets()->defaultOptions()->includePaths().insert("assets");
#endif

#ifdef DEBUG
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif
	
	// load sponza lighting effect and set it as the default effect
	sceneManager->assets()
		->load("effect/SponzaLighting.effect")
		->load("effect/Basic.effect");
	sceneManager->assets()->defaultOptions()->effect(sceneManager->assets()->effect("effect/SponzaLighting.effect"));

	// load other assets
	sceneManager->assets()
		->queue("texture/firefull.jpg")
		->queue("effect/Particles.effect")
		->queue(MK_NAME);
	
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);

	renderer = Renderer::create();

	initializePhysics();
	
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		scene::Node::Ptr mk = assets->node(MK_NAME);
		initializeCamera(mk);

		root->addChild(group);
		root->addComponent(sceneManager);
		root->addComponent(sponzaLighting);

		group->addComponent(Transform::create());
		group->addChild(mk);

		scene::NodeSet::Ptr fireNodes = scene::NodeSet::create(group)
			->descendants()
			->where([](scene::Node::Ptr node)
		{
			return node->name() == "fire";
		});

		auto fire = Fire::create(assets);
		for (auto fireNode : fireNodes->nodes())
			fireNode->addComponent(fire);
	});

	sceneManager->assets()->load();

	std::cout << "start rendering" << std::endl << std::flush;

#if defined EMSCRIPTEN
	glutSpecialFunc(keyDownHandler);
	glutSpecialUpFunc(keyUpHandler);
	glutMotionFunc(glutMouseMoveHandler);

	emscripten_set_main_loop(renderScene, 0, true);
#else
	while (!glfwWindowShouldClose(window))
	{
		auto cameraTransform = camera->component<Transform>()->transform();
		if (cameraCollider == nullptr)
		{
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				cameraTransform->prependTranslation(0.f, 0.f, -CAMERA_LIN_SPEED);
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
					 glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				cameraTransform->prependTranslation(0.f, 0.f, CAMERA_LIN_SPEED);
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				cameraTransform->prependRotation(-CAMERA_ANG_SPEED, Vector3::yAxis());
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
					 glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				cameraTransform->prependRotation(CAMERA_ANG_SPEED, Vector3::yAxis());
		}
		else
		{

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				// go forward
				cameraTransform->prependTranslation(Vector3::create(0.0f, 0.0f, -CAMERA_LIN_SPEED));
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
					 glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				// go backward
				cameraTransform->prependTranslation(Vector3::create(0.0f, 0.0f, CAMERA_LIN_SPEED));
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				cameraTransform->prependTranslation(-CAMERA_LIN_SPEED, 0.0f, 0.0f);
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
					 glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				cameraTransform->prependTranslation(CAMERA_LIN_SPEED, 0.0f, 0.0f);

			eye = cameraTransform->translationVector();

			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && eye->y() <= 0.5f)
				cameraTransform->prependTranslation(0.0f, 4 * CAMERA_LIN_SPEED, 0.0f);

			// look around
			eye = cameraTransform->translationVector();

			target->setTo(
				eye->x() + sinf(rotationY) * cosf(rotationX),
				eye->y() + sinf(rotationX),
				eye->z() + cosf(rotationY) * cosf(rotationX)
			);

			cameraTransform->view(eye, target, Vector3::upAxis());

			auto newEyePos = cameraTransform->translationVector();

			cameraTransform->appendTranslation(
				eye->x() - newEyePos->x(),
				eye->y() - newEyePos->y(),
				eye->z() - newEyePos->z()
			);

			cameraCollider->synchronizePhysicsWithGraphics();
		}
		
		sceneManager->nextFrame();
		
		sponzaLighting->step();
		renderer->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	std::exit(EXIT_SUCCESS);
#endif
}