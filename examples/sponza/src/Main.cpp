#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoMk.hpp"
#include "minko/MinkoBullet.hpp"
#include "minko/MinkoParticles.hpp"

#ifdef EMSCRIPTEN
	#include "minko/MinkoWebGL.hpp"
	#include "GL/glut.h"

	#define FRAMERATE 60
#else
	#include "GLFW/glfw3.h"
#endif

#include "minko/component/SponzaLighting.hpp"
#include "minko/component/Fire.hpp"

using namespace minko::component;
using namespace minko::math;

const float WINDOW_WIDTH        = 1024;
const float WINDOW_HEIGHT       = 500;

const float CAMERA_LIN_SPEED	= 0.05f;
const float CAMERA_ANG_SPEED	= PI * 1.0f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;
const std::string CAMERA_NAME   = "camera";

Rendering::Ptr	                rendering           = nullptr;
bullet::ColliderComponent::Ptr	cameraColliderComp  = nullptr;
auto			                sponzaLighting	    = SponzaLighting::create();
auto			                mesh			    = scene::Node::create("mesh");
auto			                group			    = scene::Node::create("group");
auto			                camera			    = scene::Node::create("camera");
auto			                root			    = scene::Node::create("root");


#ifdef EMSCRIPTEN
void
clavierHandler(unsigned char key, int x, int y)
{	
    if (cameraColliderComp == nullptr)
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
			cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, -CAMERA_LIN_SPEED));
		else if (key == GLUT_KEY_DOWN)
			cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, CAMERA_LIN_SPEED));
		if (key == GLUT_KEY_LEFT)
			cameraColliderComp->prependRotationY(CAMERA_ANG_SPEED);
		else if (key == GLUT_KEY_RIGHT)
			cameraColliderComp->prependRotationY(-CAMERA_ANG_SPEED);
	}
	 
}

void
renderScene()
{
	rendering->render();

	glutSwapBuffers();
	glutPostRedisplay();
}

void timerFunc(int)
{
	glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	glutPostRedisplay();
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

	if (shapeData.find("materialProfile") != shapeData.end())
	{
		Qark::ByteArray& materialProfileData = Any::cast<Qark::ByteArray&>(shapeData["materialProfile"]);
		std::stringstream	stream;
		stream.write(&*materialProfileData.begin(), materialProfileData.size());

		double density	= readAndSwap<double>(stream); // do not care about it at this point
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

void
printFramerate(const unsigned int delay = 1)
{
	static auto start = clock();
	static auto numFrames = 0;

	auto time = clock();
	auto deltaT = (float)(clock() - start) / CLOCKS_PER_SEC;

	++numFrames;
	if (deltaT > delay)
	{
		std::cout << ((float)numFrames / deltaT) << " fps." << std::endl;
		start = time;
		numFrames = 0;
	}
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
initializeCamera()
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
			cameraColliderComp = camera->component<component::bullet::ColliderComponent>();
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

int main(int argc, char** argv)
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
	glutCreateWindow("Minko Examples");

	std::cout << "WebGl context created" << std::endl;

	auto context = render::WebGLContext::create();
#else
    glfwInit();
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sponza Example", NULL, NULL);
    glfwMakeContextCurrent(window);

	std::cout << "OpenGl context created" << std::endl:

	auto context = render::OpenGLES2Context::create();
#endif

    std::cout << context->driverInfo() << std::endl;

    auto assets	= AssetsLibrary::create(context)
		->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context));

#ifdef EMSCRIPTEN
	assets->defaultOptions()->includePaths().insert("assets");
#endif
#ifdef NDEBUG
    assets->defaultOptions()->includePaths().insert("../..");
#endif

    // load sponza lighting effect and set it as the default effect
    assets->load("effect/SponzaLighting.effect");
    assets->defaultOptions()->effect(assets->effect("sponza lighting"));

    // load other assets
    assets
		->queue("texture/firefull.jpg")
		->queue("effect/Particles.effect")
		->queue("models/Sponza_lite.mk");

    rendering = Rendering::create(context);

    initializePhysics();

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
        initializeCamera();

       	root->addChild(group);
		root->addComponent(sponzaLighting);
		//root->addComponent(DirectionalLight::create());

		group->addComponent(Transform::create());
		group->addChild(assets->node("models/Sponza_lite.mk"));

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

	try
	{
		assets->load();
	}
	catch(std::exception e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
	}

	std::cout << "start rendering" << std::endl << std::flush;

#ifdef EMSCRIPTEN
	glutSpecialFunc(clavierHandler);
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return 0;
#else
	
	while(!glfwWindowShouldClose(window))
    {
        if (cameraColliderComp == nullptr)
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
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, -CAMERA_LIN_SPEED));
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, CAMERA_LIN_SPEED));
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				cameraColliderComp->prependRotationY(CAMERA_ANG_SPEED);
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				cameraColliderComp->prependRotationY(-CAMERA_ANG_SPEED);
		}

	    rendering->render();

		sponzaLighting->step();
	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    exit(EXIT_SUCCESS);
#endif
}