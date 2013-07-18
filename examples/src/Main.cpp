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
	double restitution	= 1;

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
	camera->component<Transform>()->transform()->appendTranslation(0.f, 0.5f, 07.5f)->appendRotationY(PI/2.f);
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
		->queue("models/testphysics5.mk");

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
		group->addChild(assets->node("models/testphysics5.mk"));
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
		renderingComponent->render();

	    glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}