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

using namespace minko::component;
using namespace minko::math;

const float CAMERA_LIN_SPEED	= 0.05f;
const float CAMERA_ANG_SPEED	= PI * 1.0f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;
const std::string CAMERA_NAME   = "camera";

Rendering::Ptr	rendering;
auto			sponzaLighting	= SponzaLighting::create();
auto			mesh			= scene::Node::create("mesh");
auto			group			= scene::Node::create("group");
auto			camera			= scene::Node::create("camera");
auto			root			= scene::Node::create("root");


#ifdef EMSCRIPTEN
void
renderScene()
{
	renderingComponent->render();

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

		        deserializedShape->setCenterOfMassOffset(offset, scaling);
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

	float mass = 1;

	double vx =  0;
	double vy =  0;
	double vz =  0;
	double avx = 0;
	double avy = 0;
	double avz = 0;
	bool sleep	= 0;
	bool rotate = 0;

	double density		= 0;
	double friction		= 0;
	double restitution	= 0.5;

	if (shapeData.find("materialProfile") != shapeData.end())
	{
		Qark::ByteArray& materialProfileData = Any::cast<Qark::ByteArray&>(shapeData["materialProfile"]);
		std::stringstream	stream;
		stream.write(&*materialProfileData.begin(), materialProfileData.size());

		density = readAndSwap<double>(stream);
		//friction = readAndSwap<double>(stream);
		//restitution = readAndSwap<double>(stream);
	}

	if (colliderData.find("dynamics") == colliderData.end())
		mass = 0;
	else
	{
		Qark::ByteArray& dynamicsData = Any::cast<Qark::ByteArray&>(colliderData["dynamics"]);
		std::stringstream	stream;
		stream.write(&*dynamicsData.begin(), dynamicsData.size());

		vx = readAndSwap<double>(stream);
		vy = readAndSwap<double>(stream);
		vz = readAndSwap<double>(stream);

		avx = readAndSwap<double>(stream);
		avy = readAndSwap<double>(stream);
		avz = readAndSwap<double>(stream);

		sleep	= readAndSwap<bool>(stream);
		rotate = readAndSwap<bool>(stream);
	}

	bullet::Collider::Ptr collider = bullet::Collider::create(mass, shape);

	collider->setAngularVelocity(avx, avy, avz);
	collider->setLinearVelocity(vx, vy, vz);
	collider->setFriction(friction);
	collider->setRestitution(restitution);

	if (rotate == false)
	{
		collider->setAngularFactor(0, 0, 0);
	}

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

ParticleSystem::Ptr
createFire(AssetsLibrary::Ptr assets)
{
	ParticleSystem::Ptr particleSystem;

#define SCALE 0.05
	particleSystem = ParticleSystem::create(
		assets->context(),
		assets,
		200,
		particle::sampler::RandomValue<float>::create(1, 1.3),
		particle::shape::Sphere::create(1 * SCALE),
		particle::StartDirection::NONE,
		0);
	particleSystem->material()
		->set("material.diffuseColor",	Vector4::create(.3f, .07f, .02f, 1.f))
		->set("material.diffuseMap",	assets->texture("texture/firefull.jpg"));


	particleSystem->add(particle::modifier::StartForce::create(
		particle::sampler::RandomValue<float>::create(-.2 * SCALE, .2 * SCALE),
		particle::sampler::RandomValue<float>::create(6. * SCALE, 8. * SCALE),
		particle::sampler::RandomValue<float>::create(-.2 * SCALE, .2 * SCALE)
		));

	particleSystem->add(particle::modifier::StartSize::create(
		particle::sampler::RandomValue<float>::create(1.3 * SCALE, 1.6 * SCALE)
		));

	particleSystem->add(particle::modifier::StartSprite::create(
		particle::sampler::RandomValue<float>::create(0. * SCALE, 4. * SCALE)
		));

	particleSystem->add(particle::modifier::StartAngularVelocity::create(
		particle::sampler::RandomValue<float>::create(0.1 * SCALE, 2. * SCALE)
		));

	particleSystem->add(particle::modifier::SizeOverTime::create());
	particleSystem->add(particle::modifier::ColorOverTime::create());

	particleSystem->updateRate(30);
	particleSystem->fastForward(2, 30);
	particleSystem->play();
	return particleSystem;
}

component::bullet::ColliderComponent::Ptr
initializeDefaultCameraCollider()
{
	bullet::BoxShape::Ptr	cameraShape	= bullet::BoxShape::create(0.2f, 0.3f, 0.2f);
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
    bullet::ColliderComponent::Ptr	cameraColliderComp = nullptr;

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
			->appendTranslation(0.0f, 2.75f, 5.0f)
			->appendRotationY(PI*0.5);

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
	camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));

    root->addChild(camera);
}

void
initializePhysics()
{
    auto physicWorld = bullet::PhysicsWorld::create();

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
	glutInitWindowSize(800, 600);
	glutCreateWindow("Minko Examples");

	auto context = render::WebGLContext::create();
#else
    glfwInit();
    auto window = glfwCreateWindow(800, 600, "Sponza Example", NULL, NULL);
    glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
#endif

    initializeCamera();

	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::PNGParser>("png")
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context));

#ifdef EMSCRIPTEN
	assets->defaultOptions()->includePaths().insert("assets");
#endif
#ifdef NDEBUG
	assets->defaultOptions()->includePaths().insert(MINKO_FRAMEWORK_EFFECTS_PATH);
    assets->defaultOptions()->includePaths().insert("../..");
#endif
#ifdef DEBUG
	assets->defaultOptions()->includePaths().insert(MINKO_FRAMEWORK_EFFECTS_PATH);
#endif

    // load sponza lighting effect and set it as the default effect
    assets->load("effect/SponzaLighting.effect");
    assets->defaultOptions()->effect(assets->effect("sponza lighting"));

    // load other assets
    assets
		->queue("texture/firefull.jpg")
		->queue("Particles.effect")
		->queue("models/sponza-lite-physics.mk");

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
       	root->addChild(group);
		root->addComponent(sponzaLighting);
		//root->addComponent(DirectionalLight::create());

		group->addComponent(Transform::create());
		group->addChild(assets->node("models/sponza-lite-physics.mk"));
		
		scene::NodeSet::Ptr fireNodes = scene::NodeSet::create(group)->descendants(false)->where(
			[](scene::Node::Ptr node)
			{
				return node->name() == "fire";
			});

		for (unsigned int i = 0; i < fireNodes->nodes().size(); ++i)
		{
			scene::Node::Ptr currentNode = fireNodes->nodes()[i];

			if (i == 3)
				currentNode->component<Transform>()->transform()->appendTranslation(0., .06, 0.);

			currentNode->addComponent(createFire(assets));
		}
	});

	try
	{
		assets->load();
	}
	catch(std::exception e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
	}

#ifdef EMSCRIPTEN
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return 0;
#else
	while(!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, -.1f);
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, .1f);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(-.1f, 0.f, 0.f);
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(.1f, 0.f, 0.f);

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