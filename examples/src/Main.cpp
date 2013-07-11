#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoBullet.hpp"
#include "minko/MinkoWebGL.hpp"

#ifdef EMSCRIPTEN
#include "GL/glut.h"
#else
#include "GLFW/glfw3.h"
#endif // EMSCRIPTEN

#define FRAMERATE 60

using namespace minko::controller;
using namespace minko::math;

RenderingController::Ptr	renderingController;
bullet::PhysicsWorld::Ptr	physicsWorld;

auto mesh		= scene::Node::create("mesh");
auto mesh2		= scene::Node::create("mesh2");
auto staticMesh	= scene::Node::create("staticMesh");
auto group		= scene::Node::create("group");
auto subgroup	= scene::Node::create("subgroup");

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

#ifdef EMSCRIPTEN
void
	glutRenderScene()
{
	//mesh->controller<TransformController>()->transform()->prependRotationY(.01f);
	renderingController->render();

	//printFramerate();

	glutSwapBuffers();
	glutPostRedisplay();

}
#endif // EMSCRIPTEN

/*void screenshotFunc(int)
{
const int width = 800, height = 600;

char* pixels = new char[3 * width * height];

glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

int i, j;
FILE *fp = fopen("screenshot.ppm", "wb");
fprintf(fp, "P6\n%d %d\n255\n", width, height);

for (j = 0; j < height; ++j)
{
for (i = 0; i < width; ++i)
{
static unsigned char color[3];
color[0] = pixels[(width * j + i) * 3 + 0];
color[1] = pixels[(width * j + i) * 3 + 1];
color[2] = pixels[(width * j + i) * 3 + 2];
(void) fwrite(color, 1, 3, fp);
}
}

fclose(fp);

delete[] pixels;
}*/

bool testQuaternion(uint ax, float ang)
{
	Vector3::Ptr	axis		= nullptr;
	Matrix4x4::Ptr	refMatrix	= Matrix4x4::create()->identity();
	switch(ax)
	{
	case 0:
		axis = Vector3::xAxis();
		refMatrix->appendRotationX(ang);
		break;

	case 1:
		axis = Vector3::yAxis();
		refMatrix->appendRotationY(ang);
		break;

	default:
		axis = Vector3::zAxis();
		refMatrix->appendRotationZ(ang);
		break;
	}

	if (ax > 2)
	{
		axis = Vector3::create(rand(), rand(), rand())->normalize();
	}

	Quaternion::Ptr	quat		= Quaternion::create()->initialize(ang, axis);
	Matrix4x4::Ptr	quatMatrix	= quat->toMatrix();
	Quaternion::Ptr	quat2		= quatMatrix->rotation();
	Matrix4x4::Ptr	quatMatrix2	= quat2->toMatrix();

	std::cout << "axis = " << axis->x() << ", " << axis->y() << ", " << axis->z() << "\tang = " << ang 
		<< "\n\t- from quat\t= " << std::to_string(quatMatrix) << "\n\t- rot\t= " << std::to_string(refMatrix)
		<< std::endl;



	std::cout << "\t-- quat  = " << quat->i() << ", " << quat->j() << ", " << quat->k() << " | " << quat->r() << std::endl;
	std::cout << "\t-- quat2 = " << quat2->i() << ", " << quat2->j() << ", " << quat2->k() << " | " << quat2->r() << std::endl;

	if (ax > 2)
	{
		for (uint i=0; i<16; ++i)
			if (fabsf(quatMatrix->values()[i] - quatMatrix2->values()[i]) > 1e-6f)
				return false;
	}
	else
	{
		for (uint i=0; i<16; ++i)
			if (fabsf(quatMatrix->values()[i] - refMatrix->values()[i]) > 1e-6f)
				return false;

		for (uint i=0; i<16; ++i)
			if (fabsf(quatMatrix2->values()[i] - refMatrix->values()[i]) > 1e-6f)
				return false;
	}
	return true;
}

int main(int argc, char** argv)
{
#ifdef EMSCRIPTEN
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Minko Examples");

	auto context = render::WebGLContext::create();
#else
	glfwInit();
	auto window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
	glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
#endif // EMSCRIPTEN

	/*
	for (float ang = -90.0f; ang < 90.0f; ang += 10.0f)
	{
	for (uint axis = 0; axis <= 3; ++axis)
	{
	if (!testQuaternion(axis, ang))
	throw std::logic_error("ouch");
	}
	}
	*/

	context->setBlendMode(render::Blending::Mode::DEFAULT);

	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::EffectParser>("effect")
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
		//->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("textures/collage.jpg")
		->queue("textures/box3.png")
		->queue("effects/Texture.effect")
		//->queue("effects/Red.effect")
		->queue("effects/Basic.effect");

	/*assets->defaultOptions()->includePath("effects");*/


	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");

		group->addController(Transform::create());
		group->controller<Transform>()->transform()->appendRotationY(10.0f*PI/180.0f);

		subgroup->addController(Transform::create());
		subgroup->controller<Transform>()->transform()->appendTranslation(-0.1, -0.2, -0.3);

		root->addChild(group)->addChild(camera);

		renderingController = RenderingController::create(assets->context());
		renderingController->backgroundColor(0x7F7F7FFF);
		camera->addController(renderingController);


		physicsWorld	= bullet::PhysicsWorld::create();
		physicsWorld->setGravity(Vector3::create(0.0f, -9.81f, 0.0f));
		root->addController(physicsWorld);


		auto view = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
		auto color = Vector4::create(0.f, 0.f, 1.f, .1f);
		auto lightDirection = Vector3::create(0.f, -1.f, -1.f);

		mesh->addController(Transform::create());
		mesh->controller<Transform>()->transform()->appendTranslation(0.f, 0.8f, -3.f);
		mesh->addController(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
			->set("material/diffuse/rgba",			color)
			->set("transform/worldToScreenMatrix",	view)
			->set("light/direction",				lightDirection)
			->set("material/diffuse/map",			assets->texture("textures/box3.png")),
			assets->effect("texture")
			));

		Quaternion::Ptr quat = Quaternion::create()
			->initialize(45.0f*PI/180.0f, Vector3::create(rand(), rand(), rand()));
		std::cout << "quat.norm = " << quat->length() << std::endl;

		Matrix4x4::Ptr quatMatrix = quat->toMatrix();
		std::cout << "determinant = " << quatMatrix->determinant3x3() << std::endl;

		mesh2->addController(Transform::create());
		mesh2->controller<Transform>()->transform()
			->identity()
			->append(quat)
			->appendTranslation(0.95f, 1.8f, -3.f);
		//->appendRotation(45.0f*PI/180.0f, Vector3::create(rand(), rand(), rand())->normalize());
		//->appendTranslation(0.8f, 1.5f, -3.f);

		std::cout << "determinant mesh 2 = " << mesh2->controller<Transform>()->transform()->determinant3x3() << std::endl; 

		mesh2->addController(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
			->set("material/diffuse/rgba",			color)
			->set("transform/worldToScreenMatrix",	view)
			->set("light/direction",				lightDirection)
			->set("material/diffuse/map",			assets->texture("textures/box3.png")),
			assets->effect("texture")
			));

		staticMesh->addController(Transform::create());
		staticMesh->controller<Transform>()->transform()->appendTranslation(0.7f, -0.8f, -3.f);
		staticMesh->addController(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
			->set("material/diffuse/rgba",			color)
			->set("transform/worldToScreenMatrix",	view)
			->set("light/direction",				lightDirection)
			->set("material/diffuse/map",			assets->texture("textures/box3.png")),
			assets->effect("texture")
			));

		auto shape		= bullet::BoxShape::create(0.5f, 0.5f, 0.5f);
		auto collider	= bullet::Collider::create(10.0f, shape);
		auto collider2	= bullet::Collider::create(0.1f, shape);

		auto staticCollider	= bullet::Collider::create(0.0f, shape);

		mesh->addController(bullet::ColliderController::create(collider));
		mesh2->addController(bullet::ColliderController::create(collider2));
		staticMesh->addController(bullet::ColliderController::create(staticCollider));

		group->addChild(subgroup);

		subgroup->addChild(mesh);
		subgroup->addChild(mesh2);
		subgroup->addChild(staticMesh);
	});

	try
	{
		assets->load();
	}
	catch(std::exception e)
	{
		std::cerr << "exception\n\t" << e.what() << std::endl;
	}


	// auto oglContext = context;
	// auto fx = assets->effect("directional light");

	// std::cout << "== vertex shader compilation logs ==" << std::endl;
	// std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->vertexShader()) << std::endl;
	// std::cout << "== fragment shader compilation logs ==" << std::endl;
	// std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->fragmentShader()) << std::endl;
	// std::cout << "== program info logs ==" << std::endl;
	// std::cout << oglContext->getProgramInfoLogs(fx->shaders()[0]->program()) << std::endl;

	//glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	//glutTimerFunc(1000, screenshotFunc, 0);

#ifdef EMSCRIPTEN
	glutDisplayFunc(glutRenderScene);
	glutMainLoop();
	return 0;
#else
	while(!glfwWindowShouldClose(window))
	{
		//mesh->controller<Transform>()->transform()->prependRotationY(.01f);

		renderingController->render();

		printFramerate();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
#endif // EMSCRIPTEN
}