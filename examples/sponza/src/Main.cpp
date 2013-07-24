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

const float WINDOW_WIDTH		= 1024;
const float WINDOW_HEIGHT		= 500;

const float CAMERA_LIN_SPEED	= 0.1f;
const float CAMERA_ANG_SPEED	= PI * 2.f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;
const std::string CAMERA_NAME	= "camera";

Rendering::Ptr		rendering			= nullptr;
auto				sponzaLighting		= SponzaLighting::create();
auto				mesh				= scene::Node::create("mesh");
auto				group				= scene::Node::create("group");
auto				camera				= scene::Node::create("camera");
auto				root				= scene::Node::create("root");
auto				speed				= 0.f;
auto				angSpeed			= 0.f;
float				_rotationX			= 0;
float				_rotationY			= 0;
float				_mousePositionX		= 0;
float				_mousePositionY		= 0;

Vector3::Ptr					_target					= Vector3::create();
Vector3::Ptr					_eye					= Vector3::create();
Matrix4x4::Ptr					_cameraWorldTransform	= Matrix4x4::create();
bullet::ColliderComponent::Ptr	_cameraColliderComp		= nullptr;
bool							_updateCameraRotation	= false;


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
	if (_cameraColliderComp == nullptr)
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
	if (speed)
		_cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, speed));
	// if (angSpeed)
	//     _cameraColliderComp->prependRotationY(angSpeed);

	if (_updateCameraRotation)
	{
		_eye = camera->component<Transform>()->transform()->translationVector();

		std::cout << _eye->x() << " " << _eye->y() << std::endl;
		_target->setTo(
			_eye->x() + sin(_rotationY) * cos(_rotationX),
			_eye->y() + 0, //sin(_rotationX),
			_eye->z() + cos(_rotationY) * cos(_rotationX)
			);

		// camera->component<Transform>()->transform()->view(_eye, _target, Vector3::upAxis());
		_cameraColliderComp->lookAt(_target, _eye, Vector3::upAxis());

		_updateCameraRotation = false;
	}

	sponzaLighting->step();
	rendering->render();

	glutSwapBuffers();
}
#else
void
glfwMouseMoveHandler(GLFWwindow* window, double x, double y)
{
	_rotationY += -(_mousePositionX - x) * .001;
	_rotationX +=  (_mousePositionY - y) * .001;

	const float limit = 89 * PI / 180;

	if (_rotationX < -limit)
		_rotationX = -limit;
	else if (_rotationX > +limit)
		_rotationX = +limit;

	_mousePositionX = x;
	_mousePositionY = y;

	_updateCameraRotation = true;
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
	deserializeShape(Qark::Map&							shapeData,
	scene::Node::Ptr&					node)
{
	int type = Any::cast<int>(shapeData["type"]);
	bullet::AbstractPhysicsShape::Ptr deserializedShape;
	std::stringstream	stream;

	double rx	= 0;
	double ry	= 0;
	double rz	= 0;
	double h	= 0;
	double r	= 0;

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
			Matrix4x4::Ptr offset	= deserialize::TypeDeserializer::matrix4x4(shapeData["delta"]);
			auto modelToWorldMatrix	= node->component<Transform>()->modelToWorldMatrix(true);
			const float scaling		= powf(modelToWorldMatrix->determinant3x3(), 1.0f/3.0f);

#ifdef DEBUG
			std::cout << "\n----------\n" << node->name() << "\t: deserialize TRANSFORMED\n\t- delta  \t= " << std::to_string(offset)
				<< "\n\t- toWorld\t= " << std::to_string(modelToWorldMatrix) << "\n\t- scaling = " << scaling << std::endl;
#endif // DEBUG

			deserializedShape->setLocalScaling(scaling);
			//deserializedShape->apply(modelToWorldMatrix);

			deserializedShape->setCenterOfMassOffset(offset, modelToWorldMatrix);
		}
		break;
	default:
		deserializedShape = nullptr;
	}

	return deserializedShape;
}

std::shared_ptr<bullet::ColliderComponent>
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
	double friction		= 0.5; // bullet's advices
	double restitution	= 0.0; // bullet's advices
	double density      = 0.0;

	if (shapeData.find("materialProfile") != shapeData.end())
	{
		Qark::ByteArray& materialProfileData = Any::cast<Qark::ByteArray&>(shapeData["materialProfile"]);
		std::stringstream	stream;
		stream.write(&*materialProfileData.begin(), materialProfileData.size());

		density         = readAndSwap<double>(stream); // do not care about it at this point
		friction		= readAndSwap<double>(stream);
		restitution		= readAndSwap<double>(stream);
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

	bullet::Collider::Ptr collider = bullet::Collider::create(mass, shape);

	collider->setLinearVelocity(vx, vy, vz);
	collider->setAngularVelocity(avx, avy, avz);
	collider->setFriction(friction);
	collider->setRestitution(restitution);

	if (!rotate)
		collider->setAngularFactor(0.0f, 0.0f, 0.0f);
	//collider->disableDeactivation(sleep == false);
	collider->disableDeactivation(true);

	return bullet::ColliderComponent::create(collider);
}

component::bullet::ColliderComponent::Ptr
	initializeDefaultCameraCollider()
{
	bullet::BoxShape::Ptr	cameraShape	= bullet::BoxShape::create(0.2f, .75f, 0.2f);
	//cameraShape->setMargin(0.3f);
	auto cameraCollider					= bullet::Collider::create(CAMERA_MASS, cameraShape);

	cameraCollider->setRestitution(0.5f);
	cameraCollider->setAngularFactor(0.0f, 0.0f, 0.0f);
	cameraCollider->setFriction(CAMERA_FRICTION);
	cameraCollider->disableDeactivation(true);

	return bullet::ColliderComponent::create(cameraCollider);
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

		if (camera->hasComponent<component::bullet::ColliderComponent>())
		{
			std::cout << "PARSED CAMERA & COLLIDER" << std::endl;
			_cameraColliderComp = camera->component<component::bullet::ColliderComponent>();
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
		->queue("model/Sponza_lite_sphere.mk");
	
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	
	rendering = Rendering::create(context);

	initializePhysics();
	
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		scene::Node::Ptr sponza = assets->node("model/Sponza_lite_sphere.mk");
		//scene::Node::Ptr sponza = assets->node("model/four-squares.mk");
		initializeCamera(sponza);

		root->addChild(group);
		root->addComponent(sceneManager);
		root->addComponent(sponzaLighting);

		group->addComponent(Transform::create());
		group->addChild(sponza);

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

	emscripten_set_main_loop(renderScene, 0, true);
#else
	while(!glfwWindowShouldClose(window))
	{
		if (_cameraColliderComp == nullptr)
		{
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, -CAMERA_LIN_SPEED);
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, CAMERA_LIN_SPEED);
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				camera->component<Transform>()->transform()->prependRotation(-CAMERA_ANG_SPEED, Vector3::yAxis());
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				camera->component<Transform>()->transform()->prependRotation(CAMERA_ANG_SPEED, Vector3::yAxis());
		}
		else
		{
			// the camera has a collider component

			_cameraWorldTransform = _cameraColliderComp->getPhysicsWorldTransform();

			// move forward/backward
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				_cameraWorldTransform->prependTranslation(0.f, 0.f, -CAMERA_LIN_SPEED);
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				_cameraWorldTransform->prependTranslation(0.f, 0.f, CAMERA_LIN_SPEED);

			// look around
			_eye = _cameraWorldTransform->translationVector();

			_target->setTo(
				_eye->x() + sinf(_rotationY) * cosf(_rotationX),
				_eye->y() + sinf(_rotationX),
				_eye->z() + cosf(_rotationY) * cosf(_rotationX)
				);

			_cameraWorldTransform->lookAt(_target, _eye, Vector3::upAxis());

			auto newEyePos = _cameraWorldTransform->translationVector();

			_cameraWorldTransform->appendTranslation(
				_eye->x() - newEyePos->x(),
				_eye->y() - newEyePos->y(),
				_eye->z() - newEyePos->z()
				);

			newEyePos = _cameraWorldTransform->translationVector(newEyePos);

			//camera->component<Transform>()->transform()->copyFrom(_cameraWorldTransform);
			_cameraColliderComp->setPhysicsWorldTransform(_cameraWorldTransform);

			//_updateCameraRotation = false;
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