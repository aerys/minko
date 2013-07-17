#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoMk.hpp"
#include "minko/MinkoBullet.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

using namespace minko::component;
using namespace minko::math;

Rendering::Ptr renderingComponent;
auto mesh	= scene::Node::create("mesh");
auto group	= scene::Node::create("group");
auto camera	= scene::Node::create("camera");
auto root   = scene::Node::create("root");

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
				deserializedShape = deserializeShape(Any::cast<Qark::Map&>(shapeData["subGeometry"]), node);
				Matrix4x4::Ptr offset	= deserialize::TypeDeserializer::matrix4x4(shapeData["delta"]);
				//deserializedShape->localScaleX(2.0);
				//deserializedShape->localScaleY(2.0);
				//deserializedShape->localScaleZ(2.0);
				
				
				std::cout << "\n\ndelta matrix = " << std::to_string(offset) << std::endl;
				/*
				if (offset->data()[0] > 10.0f)
				{
					std::cout << "local scale !" << std::endl;
					// i am the box
					deserializedShape->localScaleX(offset->data()[0] * 0.5);
					deserializedShape->localScaleY(offset->data()[5] * 0.5);
					deserializedShape->localScaleZ(offset->data()[10] * 0.5);
				}
				*/

				// fix : should be merged

				std::cout << node->name() << " : " << std::to_string(node->component<Transform>()->modelToWorldMatrix(true)) << std::endl;

				deserializedShape->apply(node->component<Transform>()->modelToWorldMatrix(true));
				deserializedShape->setCenterOfMassOffset(offset);
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
	double restitution	= 0;

	if (shapeData.find("materialProfile") != shapeData.end())
	{
		Qark::ByteArray& materialProfileData = Any::cast<Qark::ByteArray&>(shapeData["materialProfile"]);
		std::stringstream	stream;
		stream.write(&*materialProfileData.begin(), materialProfileData.size());

		density = readAndSwap<double>(stream);
		friction = readAndSwap<double>(stream);
		restitution = readAndSwap<double>(stream);
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

void
testMk(AssetsLibrary::Ptr assets)
{
	std::shared_ptr<data::HalfEdgeCollection> halfEdgeCollection = data::HalfEdgeCollection::create(assets->geometry("cube")->indices());
}

int main(int argc, char** argv)
{
	file::MkParser::registerController("colliderController", std::bind(deserializeBullet, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
    glfwMakeContextCurrent(window);
	auto context = render::OpenGLES2Context::create();

	root->addChild(group)->addChild(camera);
		
	renderingComponent = Rendering::create(context);
    renderingComponent->backgroundColor(0x7F7F7FFF);
	camera->addComponent(renderingComponent);
    camera->addComponent(Transform::create());
	camera->component<Transform>()->transform()->appendTranslation(0.f, 0.0f, 50.0f)->appendRotationY(0.0f);
    camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));

	auto physicWorld = bullet::PhysicsWorld::create();

	physicWorld->setGravity(math::Vector3::create(0.f, -9.8f, 0.f));
	root->addComponent(physicWorld);

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
		->queue("models/testphysics2.mk");

	//#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
	//#else
	//assets->defaultOptions()->includePaths().push_back("../../effect");
	//assets->defaultOptions()->includePaths().push_back("../../texture");
	//#endif

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
        root->addComponent(DirectionalLight::create());
		group->addComponent(Transform::create());
		group->addChild(mesh);
//		mesh->addComponent(Transform::create());
//		mesh->addComponent(Surface::create(
//			assets->geometry("cube"),
//			data::Provider::create()
//				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
//                ->set("material.diffuseMap",	assets->texture("box3.png"))
//               ->set("material.specular",	    Vector3::create(.25f, .25f, .25f))
//               ->set("material.shininess",	    30.f),
//			assets->effect("directional light")));

		group->addChild(assets->node("models/testphysics2.mk"));
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
		//camera->component<Transform>()->transform()->appendTranslation(0.f, 0.f, 0.01f);
       //group->component<Transform>()->transform()->prependRotationY(.01f);
		renderingComponent->render();

	   // printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}