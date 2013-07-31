#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoMk.hpp"
#include "minko/MinkoBullet.hpp"
#include "minko/MinkoParticles.hpp"

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

using namespace minko::component;
using namespace minko::math;

const float WINDOW_WIDTH		= 1024.0f;
const float WINDOW_HEIGHT		= 500.0f;

//const std::string MK_NAME			= "model/trigger-test.mk";
//const std::string DEFAULT_EFFECT	= "effect/Basic.effect";
const std::string MK_NAME			= "model/Sponza_lite_sphere.mk";
const std::string DEFAULT_EFFECT	= "effect/SponzaLighting.effect";
const std::string CAMERA_NAME		= "camera";

const float CAMERA_LIN_SPEED	= 0.05f;
const float CAMERA_ANG_SPEED	= PI * 2.f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;

Rendering::Ptr			rendering			= nullptr;
auto					sponzaLighting		= SponzaLighting::create();
auto					mesh				= scene::Node::create("mesh");
auto					group				= scene::Node::create("group");
auto					camera				= scene::Node::create("camera");
auto					root				= scene::Node::create("root");
auto					speed				= 0.0f;
auto					angSpeed			= 0.0f;
float					_rotationX			= 0.0f;
float					_rotationY			= 0.0f;
float					_mousePositionX		= 0.0f;
float					_mousePositionY		= 0.0f;
Vector3::Ptr			_target				= Vector3::create();
Vector3::Ptr			_eye				= Vector3::create();
bullet::Collider::Ptr	_cameraCollider		= nullptr;
//bool					_updateCameraRotation	= false;


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
	if (_cameraCollider == nullptr)
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
	_rotationY += -(_mousePositionX - x) * .005;
	_rotationX +=  (_mousePositionY - y) * .005;

	const float limit = 89 * PI / 180;

	if (_rotationX < -limit)
		_rotationX = -limit;
	else if (_rotationX > +limit)
		_rotationX = +limit;

	_mousePositionX = x;
	_mousePositionY = y;

	_updateCameraRotation = true;
}

void
renderScene()
{
	auto cameraTransform = camera->component<Transform>()->transform();
	if (_cameraCollider == nullptr)
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
		_eye = cameraTransform->translationVector();

		_target->setTo(
			_eye->x() + sinf(_rotationY) * cosf(_rotationX),
			_eye->y() + sinf(_rotationX),
			_eye->z() + cosf(_rotationY) * cosf(_rotationX)
			);
		
		// _cameraWorldTransform->lookAt(_target, _eye, Vector3::upAxis());
		cameraTransform->view(_eye, _target, Vector3::upAxis());
		
		auto newEyePos = cameraTransform->translationVector();
		
		cameraTransform->appendTranslation(
			_eye->x() - newEyePos->x(),
			_eye->y() - newEyePos->y(),
			_eye->z() - newEyePos->z()
			);
		
		_cameraCollider->synchronizePhysicsWithGraphics();
	}

	sponzaLighting->step();
	rendering->render();

	glutSwapBuffers();
}
#else
void
glfwMouseMoveHandler(GLFWwindow* window, double x, double y)
{
	_rotationY += (_mousePositionX - x) * .005;
	_rotationX += (_mousePositionY - y) * .005;

	const float limit = 89 * PI / 180;

	if (_rotationX < -limit)
		_rotationX = -limit;
	else if (_rotationX > +limit)
		_rotationX = +limit;

	_mousePositionX = x;
	_mousePositionY = y;

	//_updateCameraRotation = true;
}
#endif

template <typename T>
static void
	read(std::stringstream& stream, T& value)
{
	stream.read(reinterpret_cast<char*>(&value), sizeof (T));
}

template <typename T>
static
	T swap_endian(T u)
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
	bullet::BoxShape::Ptr	cameraShape	= bullet::BoxShape::create(0.2f, 0.3f, 0.2f);
	//cameraShape->setMargin(0.3f);
	auto cameraCollider					= bullet::ColliderData::create(CAMERA_MASS, cameraShape);
	cameraCollider->restitution(0.5f);
	cameraCollider->angularFactor(0.0f, 0.0f, 0.0f);
	cameraCollider->friction(CAMERA_FRICTION);
	cameraCollider->disableDeactivation(true);
	
	return bullet::Collider::create(cameraCollider);
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
		
		cameraColliderComp = initializeDefaultCameraCollider();
		camera->addComponent(cameraColliderComp);
	}
	else
	{
		// set-up camera from the mk file
		camera = cameras->nodes().front();
		cameraInGroup = true;

		std::cout << "parsed camera's transform = " << std::to_string(camera->component<Transform>()->transform()) << std::endl;

		if (camera->hasComponent<component::bullet::Collider>())
		{
			std::cout << "PARSED CAMERA & COLLIDER" << std::endl;
			_cameraCollider = camera->component<component::bullet::Collider>();
		}
		else
			std::cout << "PARSED CAMERA W/OUT COLLIDER" << std::endl;
	}

	if (!camera->hasComponent<Transform>())
		throw std::logic_error("Camera (deserialized or created) must have a Transform.");

	camera->addComponent(rendering);
	camera->addComponent(PerspectiveCamera::create(.785f, WINDOW_WIDTH / WINDOW_HEIGHT, .1f, 1000.f));

	root->addChild(camera);
}

void
initializePhysics()
{
	auto physicWorld = bullet::PhysicsWorld::create(rendering);

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

	rendering = Rendering::create(context);

	initializePhysics();
	
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		scene::Node::Ptr mk = assets->node(MK_NAME);
		//scene::Node::Ptr mk = assets->node("model/four-squares.mk");
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
	while(!glfwWindowShouldClose(window))
	{
		auto cameraTransform = camera->component<Transform>()->transform();
		if (_cameraCollider == nullptr)
		{
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				cameraTransform->prependTranslation(0.f, 0.f, -CAMERA_LIN_SPEED);
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				cameraTransform->prependTranslation(0.f, 0.f, CAMERA_LIN_SPEED);
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				cameraTransform->prependRotation(-CAMERA_ANG_SPEED, Vector3::yAxis());
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				cameraTransform->prependRotation(CAMERA_ANG_SPEED, Vector3::yAxis());
		}
		else
		{

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				// go forward
				cameraTransform->prependTranslation(Vector3::create(0.0f, 0.0f, -CAMERA_LIN_SPEED));
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				// go backward
				cameraTransform->prependTranslation(Vector3::create(0.0f, 0.0f, CAMERA_LIN_SPEED));

			// look around
			_eye = cameraTransform->translationVector();

			_target->setTo(
				_eye->x() + sinf(_rotationY) * cosf(_rotationX),
				_eye->y() + sinf(_rotationX),
				_eye->z() + cosf(_rotationY) * cosf(_rotationX)
				);

			// _cameraWorldTransform->lookAt(_target, _eye, Vector3::upAxis());
			cameraTransform->view(_eye, _target, Vector3::upAxis());

			auto newEyePos = cameraTransform->translationVector();

			cameraTransform->appendTranslation(
				_eye->x() - newEyePos->x(),
				_eye->y() - newEyePos->y(),
				_eye->z() - newEyePos->z()
				);

			_cameraCollider->synchronizePhysicsWithGraphics();
		}
		
		sceneManager->nextFrame();
		
		sponzaLighting->step();
		rendering->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	std::exit(EXIT_SUCCESS);
#endif
}