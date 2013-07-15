#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoBullet.hpp"

#ifdef EMSCRIPTEN
#include "minko/MinkoWebGL.hpp"
#include "GL/glut.h"
#else
#include "GLFW/glfw3.h"
#endif // EMSCRIPTEN

#define FRAMERATE 60

using namespace minko::controller;
using namespace minko::math;

RenderingController::Ptr	renderingController;
bullet::PhysicsWorld::Ptr	physicsWorld;

std::vector<scene::Node::Ptr> boxes(50);
auto staticBox	= scene::Node::create("staticBox");
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

void testMatrixProduct()
{
	std::vector<float> values1(16);
	std::vector<float> values2(16);
	for (unsigned int i=0; i<16; ++i)
	{
		values1[i] = (float)(rand() % 20);
		values2[i] = (float)(rand() % 20);
	}

	auto mat1	= math::Matrix4x4::create()
		->initialize(
		values1[0], values1[1], values1[2], values1[3],
		values1[4], values1[5], values1[6], values1[7],
		values1[8], values1[9], values1[10], values1[11],
		values1[12], values1[13], values1[14], values1[15]
	);
	auto mat2	= math::Matrix4x4::create()
		->initialize(
		values2[0], values2[1], values2[2], values2[3],
		values2[4], values2[5], values2[6], values2[7],
		values2[8], values2[9], values2[10], values2[11],
		values2[12], values2[13], values2[14], values2[15]
	);

	auto matP	= mat1 * mat2;
	std::cout << "M1 = " << std::to_string(mat1) << "\nM2 = " << std::to_string(mat2) << "\nM1*M2 = " << std::to_string(matP) << std::endl;
}

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

	//testMatrixProduct();

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
<<<<<<< HEAD
		//->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("textures/collage.jpg")
		->queue("textures/box3.png")
		->queue("effects/Texture.effect")
		//->queue("effects/Red.effect")
		->queue("effects/Basic.effect");

	/*assets->defaultOptions()->includePath("effects");*/

=======
		->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("collage.jpg")
        ->queue("box3.png")
		->queue("DirectionalLight.effect");
		//->queue("VertexNormal.effect")
		//->queue("Texture.effect")
		//->queue("Red.effect")
		//->queue("Basic.effect");

#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
#else
	assets->defaultOptions()->includePaths().push_back("../../effect");
	assets->defaultOptions()->includePaths().push_back("../../texture");
#endif
>>>>>>> 443a09b... minor fixes and add triangle culling render state

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		const float ZTRANSL = -10.0f;

		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");

		group->addController(Transform::create());
		group->controller<Transform>()->transform()->appendRotationY(0.5f*PI/180.0f);

		subgroup->addController(Transform::create());
		subgroup->controller<Transform>()->transform()->appendTranslation(-0.1, 0.1, 0.0);

		root->addChild(group)->addChild(camera);

		renderingController = RenderingController::create(assets->context());
		renderingController->backgroundColor(0x7F7F7FFF);
		camera->addController(renderingController);

<<<<<<< HEAD

		physicsWorld	= bullet::PhysicsWorld::create();
		physicsWorld->setGravity(Vector3::create(0.0f, -9.81f, 0.0f));
		root->addController(physicsWorld);

=======
        auto view = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f)->prependTranslation(0.f, 0.f, -3.f);
		auto color = Vector4::create(0.f, 0.f, 1.f, 1.f);
		auto lightDirection = Vector3::create(-1.f, 0.f, 0.f);

		mesh->addController(Transform::create());
		//mesh->controller<Transform>()->transform()->appendTranslation(0.f, 0.f, -3.f);
		mesh->addController(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuse.rgba",			color)
				->set("transform.worldToScreenMatrix",	view)
				->set("light.ambient.rgba",				Vector3::create(.25f, .25f, .25f))
				->set("light.direction",				lightDirection),
			assets->effect("directional light")
		));

		group->addChild(mesh);

<<<<<<< HEAD
        /*
		mesh = scene::Node::create();
		mesh->addController(Transform::create());
		mesh->controller<Transform>()->transform()->appendTranslation(-.75f, 0.f, 0.f);
		mesh->addController(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
				->set("material/diffuse/rgba",			color)
                ->set("material/phong/exponent",        50.f)
				->set("transform/worldToScreenMatrix",	view)
				->set("light/direction",				lightDirection)
				->set("light/ambient/rgba",				Vector3::create(.25f, .25f, .25f))
				->set("material/diffuse/map",			assets->texture("box3.png")),
			assets->effect("directional light")
		));
        */

		//group->addChild(mesh);
>>>>>>> 443a09b... minor fixes and add triangle culling render state

		auto view = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
		auto color = Vector4::create(0.f, 0.f, 1.f, .1f);
		auto lightDirection = Vector3::create(0.f, -1.f, -1.f);

		group->addChild(subgroup);


		auto shape = bullet::BoxShape::create(0.5f, 0.5f, 0.5f);

		for (unsigned int i=0; i<boxes.size(); ++i)
		{
			std::stringstream stream;
			stream << "box_" << i;

			float	ang		= (2.0f*(rand()/(float)RAND_MAX) - 1.0f)*PI;
			auto	axis	= Vector3::create(rand(), rand(), rand())->normalize();
			auto	transl	= Vector3::create(i%2==0 ? 0.55f : -0.55f, i*1.1f, ZTRANSL);

			boxes[i] = scene::Node::create(stream.str());

			boxes[i]->addController(Transform::create());
			boxes[i]->controller<Transform>()->transform()
				->appendRotation(ang, axis)
				->appendTranslation(transl->x(), transl->y(), transl->z());

			boxes[i]->addController(Surface::create(
				assets->geometry("cube"),
				data::Provider::create()
				->set("material/diffuse/rgba",			color)
				->set("transform/worldToScreenMatrix",	view)
				->set("light/direction",				lightDirection)
				->set("material/diffuse/map",			assets->texture("textures/box3.png")),
				assets->effect("texture")
				));

			boxes[i]->addController(bullet::ColliderController::create(
				bullet::Collider::create(0.1f + (rand()/(float)RAND_MAX)*10.0f, shape)
				));

			subgroup->addChild(boxes[i]);
		}

		staticBox->addController(Transform::create());
		staticBox->controller<Transform>()->transform()
			->appendTranslation(0.0f, -0.5f, ZTRANSL);

		staticBox->addController(Surface::create(
=======
        root->addComponent(DirectionalLight::create());

        group->addChild(mesh);

		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
>>>>>>> 7695b92... fix broken reference to the "transform.worldToScreenMatrix" property
			assets->geometry("cube"),
			data::Provider::create()
			->set("material/diffuse/rgba",			color)
			->set("transform/worldToScreenMatrix",	view)
			->set("light/direction",				lightDirection)
			->set("material/diffuse/map",			assets->texture("textures/box3.png")),
			assets->effect("texture")
			));

		staticBox->addController(bullet::ColliderController::create(
			bullet::Collider::create(0.0f, shape)
			));

		subgroup->addChild(staticBox);
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
<<<<<<< HEAD
	{
		//mesh->controller<Transform>()->transform()->prependRotationY(.01f);

		renderingController->render();
=======
    {
        //group->controller<Transform>()->transform()->appendRotationY(.01f);
        mesh->controller<Transform>()->transform()->prependRotationY(.01f);
>>>>>>> 443a09b... minor fixes and add triangle culling render state

		printFramerate();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
#endif // EMSCRIPTEN
}