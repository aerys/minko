#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoMk.hpp"
#include "minko/MinkoBullet.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

#define SHOW_SPONZA true

using namespace minko::component;
using namespace minko::math;

const std::string cameraName	= "camera";
//const std::string cameraName	= "Camera_1";
//const std::string mkFilename	= "models/camera-collider-bis.mk";
const std::string mkFilename = "models/sponza-lite-physics-final.mk";

const float CAMERA_LIN_SPEED	= 0.05f;
const float CAMERA_ANG_SPEED	= PI * 1.0f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;

Rendering::Ptr	rendering = nullptr;
//bullet::PhysicsWorld::Ptr	physicsWorld = nullptr;
//bullet::Collider::Ptr		cameraCollider = nullptr;
bullet::ColliderComponent::Ptr	cameraColliderComp = nullptr;

scene::Node::Ptr camera	= nullptr;
auto root   = scene::Node::create("root");
auto mesh	= scene::Node::create("mesh");
auto group	= scene::Node::create("group");

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
static
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

			std::cout << "radius : " << r << std::endl;
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
			const float scaling		= powf(fabsf(modelToWorldMatrix->determinant3x3()), 1.0f/3.0f);

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

void
testMk(AssetsLibrary::Ptr assets)
{
	std::shared_ptr<data::HalfEdgeCollection> halfEdgeCollection = data::HalfEdgeCollection::create(assets->geometry("cube")->indices());
}

static
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

int 
main(int argc, char** argv)
{
	file::MkParser::registerController(
		"colliderController", 
		std::bind(deserializeBullet, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
		);

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
	glfwMakeContextCurrent(window);
	auto context = render::OpenGLES2Context::create();

	rendering = Rendering::create(context);
	rendering->backgroundColor(0x7F7F7FFF);

	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("box3.png")
		->queue("DirectionalLight.effect")
		->queue("VertexNormal.effect")
		->queue("Texture.effect")
		->queue("Red.effect")
		->queue("Basic.effect")
		->queue(mkFilename);

	//#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
	//#else
	//assets->defaultOptions()->includePaths().push_back("../../effect");
	//assets->defaultOptions()->includePaths().push_back("../../texture");
	//#endif


	// initialization and addition of the PhysicsWorld must come first
	auto physicsWorld = bullet::PhysicsWorld::create(rendering);
	physicsWorld->setGravity(math::Vector3::create(0.f, -9.8f, 0.f));
	root->addComponent(physicsWorld);


	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		group->addChild(assets->node(mkFilename));
		/*
#ifdef SHOW_SPONZA
		group->addChild(assets->node("models/sponza-lite-physics.mk"));
#else
		group->addChild(assets->node("models/test-ground.mk"));
#endif // SHOW_SPONZA
		*/

		auto cameras = scene::NodeSet::create(group)
			->descendants(true)
			->where([](scene::Node::Ptr node)
		{ 
			return node->name() == cameraName; 
		});

		bool cameraInGroup = false;
		if (cameras->nodes().empty())
		{
			std::cout << "MANUAL CAMERA" << std::endl;

			// default camera
			camera = scene::Node::create(cameraName);

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

		//std::cout << "camera\n\t- transform = " << std::to_string(camera->component<Transform>()->transform()) << std::endl;

		/*
		camera->addComponent(rendering);
		camera->addComponent(Transform::create());

		#ifdef SHOW_SPONZA
		// sponza-adapted camera
		camera->component<Transform>()->transform()
		->lookAt(
		Vector3::create(1.0f, 0.6f, 0.0f),
		Vector3::create(0.0f, 0.6f, 0.0f),
		Vector3::yAxis()
		);
		#else
		camera->component<Transform>()->transform()
		->appendTranslation(0.0f, 2.75f, 5.0f)
		->appendRotationY(PI*0.5);
		#endif // SHOW_SPONZA

		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
		*/

		root->addComponent(DirectionalLight::create());
		group->addComponent(Transform::create());

		if (!cameraInGroup)
			root->addChild(camera); // root must have a Rendering before adding the group !

		/*
		#ifdef SHOW_SPONZA
		group->addChild(assets->node("models/sponza-lite-physics.mk"));
		#else
		group->addChild(assets->node("models/test-ground.mk"));
		#endif // SHOW_SPONZA
		*/

		root->addChild(group);

		/*
		bullet::BoxShape::Ptr	cameraShape	= bullet::BoxShape::create(0.2f, 0.3f, 0.2f);
		//cameraShape->setMargin(0.3f);
		auto cameraCollider					= bullet::Collider::create(CAMERA_MASS, cameraShape);
		cameraCollider->setRestitution(0.5f);
		cameraCollider->setAngularFactor(0.0f, 0.0f, 0.0f);
		cameraCollider->setFriction(CAMERA_FRICTION);
		cameraCollider->disableDeactivation(true);

		cameraColliderComp = bullet::ColliderComponent::create(cameraCollider);
		camera->addComponent(cameraColliderComp);
		*/
	});

	try
	{
		assets->load();
	}
	catch(std::exception e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}

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
				// go forward
					cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, -CAMERA_LIN_SPEED));
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				// go backward
				cameraColliderComp->prependLocalTranslation(Vector3::create(0.0f, 0.0f, CAMERA_LIN_SPEED));

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				// look on the left
					cameraColliderComp->prependRotationY(CAMERA_ANG_SPEED);
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				// look on the right
				cameraColliderComp->prependRotationY(-CAMERA_ANG_SPEED);
		}
		
		rendering->render();

		/*
		if (cameraColliderComp != nullptr)
		{
			const std::vector<float>& m(camera->component<Transform>()->transform()->values());
			std::cout << "camera.pos = " << m[3] << ", " << m[7] << ", " << m[11] << std::endl;
		}
		*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}