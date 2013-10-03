#include <time.h>
#include <random>
#include <chrono>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoMk.hpp"

#ifdef EMSCRIPTEN
# include "SDL/SDL.h"
#else
# include "Leap/Leap.h"
# include "SDL2/SDL.h"
# ifdef MINKO_ANGLE
#  include "SDL2/SDL_syswm.h"
#  include <EGL/egl.h>
#  include <GLES2/gl2.h>
#  include <GLES2/gl2ext.h>
# endif
#endif

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string MK_NAME			= "model/airplane_engine_n.mk";
const std::string DEFAULT_EFFECT	= "effect/Basic.effect";

void explode(scene::Node::Ptr node, float magnitude)
{
	for (scene::Node::Ptr child : node->children())
	{
		if (child->component<Transform>())
		{
			auto cTransform = child->component<Transform>()->transform();
			auto direction = cTransform->translation();
			cTransform->appendTranslation(direction * magnitude);
			explode(child, magnitude * 0.8f);
		}
		else
			explode(child, magnitude);
	}
}

class boundingBox
{
public:
    Vector3::Ptr        corner1;
    Vector3::Ptr        corner2;
    scene::Node::Ptr    node;
};

//std::vector<boundingBox>
void
getBoundingBoxes(scene::Node::Ptr node, std::vector<boundingBox>& boxList)
{
    auto surface = node->component<Surface>();
    if (surface != nullptr)
    {
        auto geometry = surface->geometry();
        auto provider = geometry->data();
        auto vertices = provider->get<render::VertexBuffer::Ptr>("geometry.vertex.attribute.position")->data();
        Vector3::Ptr corner1 = Vector3::create(vertices[0], vertices[1], vertices[2]);
        Vector3::Ptr corner2 = Vector3::create(vertices[0], vertices[1], vertices[2]);
        
        for (int i = 0; i < vertices.size(); i += 3)
        {
            if (vertices[i] > corner1->x())
                corner1->x(vertices[i]);
            if (vertices[i+1] > corner1->y())
                corner1->y(vertices[i+1]);
            if (vertices[i+2] > corner1->z())
                corner1->z(vertices[i+2]);
            
            if (vertices[i] < corner2->x())
                corner2->x(vertices[i]);
            if (vertices[i+1] < corner2->y())
                corner2->y(vertices[i+1]);
            if (vertices[i+2] < corner2->z())
                corner2->z(vertices[i+2]);
        }
        boundingBox *box = new boundingBox();
        
        box->corner1 = corner1;
        box->corner2 = corner2;
        box->node = node;
        boxList.push_back(*box);
    }
    for (scene::Node::Ptr child : node->children())
        getBoundingBoxes(child, boxList);
}
scene::Node::Ptr RandomScene(scene::Node::Ptr base, int depth, float radius,file::AssetLibrary::Ptr assets)
{
	if (depth == 0)
		return base;
	std::random_device rd;
	std::default_random_engine generator (rd());
	std::uniform_real_distribution<float> distribution(0.f, 1.f);
	auto randf = std::bind (distribution, generator);

	int numObjects = rand() % 2  + 4;
	for (int i = 0; i < numObjects; i++)
	{
		auto node = scene::Node::create();

		int type = rand() % 2;
		if (type == 0)
			node->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
			->set("material.diffuseColor",	Vector4::create(randf(), randf(), randf(), 1.f)),
			assets->effect("effect/Basic.effect")
			));
		else
			node->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor",	Vector4::create(randf(), randf(), randf(), 1.f)),
			assets->effect("effect/Basic.effect")
			));

		node->addComponent(Transform::create());
		auto position = Vector3::create(randf() * 2.f - 1.f, randf() * 2.f - 1.f, randf() * 2.f - 1.f);
		position = position * radius;
		node->component<Transform>()->transform()->appendTranslation(position)->prependScale(depth / 2.f, depth / 2.f, depth / 2.f);
		base->addChild(node);
		RandomScene(node, depth - 1, radius / 2, assets);
		}
	return base;
}

void swap(float *v1, float *v2)
{
    float tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

scene::Node::Ptr
getTouchedMesh(scene::Node::Ptr camera, scene::Node::Ptr pointer, std::vector<boundingBox>& boxList)
{
    scene::Node::Ptr result;
    std::vector<scene::Node::Ptr> touchedNodes;

    for (boundingBox box : boxList)
    {
        auto nodeTransform = box.node->component<Transform>();
        auto modelToWorldMat = nodeTransform->modelToWorldMatrix();
        auto worldToModel = modelToWorldMat->invert();
        
        auto modelCameraTransform = Matrix4x4::create(camera->component<Transform>()->transform());
        modelCameraTransform->append(worldToModel);
        
        auto modelPointerTransform = Matrix4x4::create(pointer->component<Transform>()->transform());
        modelPointerTransform->append(worldToModel);
        
        auto rayOriginTransform = Matrix4x4::create(pointer->component<Transform>()->transform());
        rayOriginTransform->appendTranslation(0.f, 0.f, 20.f);
        rayOriginTransform->append(worldToModel);
        
        auto rayStart = rayOriginTransform->translation();
        auto pointerTranslation =modelPointerTransform->translation();
        auto direction = (modelPointerTransform->translation() - rayOriginTransform->translation())->normalize();
        
        float t0x, t0y, t0z,t1x, t1y, t1z;
        auto corner1 = box.corner1;
        auto corner2 = box.corner2;
        
        Vector3::Ptr nearCorner, farCorner;
        if (corner1->z() > corner2->z())
        {
            nearCorner = corner1;
            farCorner = corner2;
        }
        else
        {
            nearCorner = corner2;
            farCorner = corner1;
        }
        float tmin, tmax;
        t0x = (nearCorner->x() - rayStart->x()) / direction->x();
        t1x = (farCorner->x() - rayStart->x()) / direction->x();
        if (t0x > t1x) swap(&t0x, &t1x);
        tmin = t0x;
        tmax = t1x;
        
        t0y = (nearCorner->y() - rayStart->y()) / direction->y();
        t1y = (farCorner->y() - rayStart->y()) / direction->y();
        if (t0y > t1y) swap(&t0y, &t1y);
        if (t0y > tmax || tmin > t1y)
            continue;
        if (t0y > tmin)
            tmin = t0y;
        if (t1y < tmax)
            tmax = t1y;
        
        t0z = (nearCorner->z() - rayStart->z()) / direction->z();
        t1z = (farCorner->z() - rayStart->z()) / direction->z();
        if (t0z > t1z) swap(&t0z, &t1z);
        if (t0z > tmax || tmin > t1z)
            continue;
        if (t0z > tmin)
            tmin = t0z;
        if (t1z < tmax)
            tmax = t1z;
        
        //result = box.node;
        touchedNodes.push_back(box.node);
    }
    if (touchedNodes.size() > 0)
        result = touchedNodes[0];
    
    for (int i= 0; i < touchedNodes.size(); i++)
    {
//        auto touchedNodeTransform = touchedNodes[i]->component<Transform>();
//        auto resultNodeTransform = result->component<Transform>();
        
        auto touchedNodeTransform = Matrix4x4::create(touchedNodes[i]->component<Transform>()->transform());
        auto resultNodeTransform = Matrix4x4::create(result->component<Transform>()->transform());

        auto touchedNodeZ = touchedNodeTransform->append(touchedNodes[i]->component<Transform>()->modelToWorldMatrix())->translation()->z();
        auto resultNodetZ = resultNodeTransform->append(result->component<Transform>()->modelToWorldMatrix())->translation()->z();
        
        if (touchedNodeZ > resultNodetZ)
            result = touchedNodes[i];
    }
    return result;
}

int main(int argc, char** argv)
{
	std::vector<boundingBox>* boxList = new std::vector<boundingBox>();
    
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		exit(-1);

#ifdef EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Minko - Cube Example", "Minko");
	SDL_Surface *screen = SDL_SetVideoMode(800,
		600,
		0, SDL_OPENGL);

	std::cout << "WebGL context created" << std::endl;
	//context = render::WebGLContext::create();
#else
	SDL_Window* window = SDL_CreateWindow(
		"Minko - Cube Example",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_OPENGL
	);

# ifdef MINKO_ANGLE
	ESContext* context;
	if (!(context = initContext(window)))
		throw std::runtime_error("Could not create eglContext");

	std::cout << "EGLContext Initialized" << std::endl;
# else
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
# endif
#endif

	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());
	auto baseNode = scene::Node::create("baseNode");
	auto ambientLightNode	= scene::Node::create("ambientLight");
	auto pointer = scene::Node::create("pointer");
	auto selectedMesh = baseNode;

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context()));

#ifdef DEBUG
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else 
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif

	auto options = sceneManager->assets()->defaultOptions();

	options->material(data::Provider::create()->set("material.triangleCulling", render::TriangleCulling::FRONT));
	options->generateMipmaps(true);

		sceneManager->assets()
		->load("effect/Phong.effect")
		->load("effect/Basic.effect");

	options->effect(sceneManager->assets()->effect("effect/Phong.effect"));

	sceneManager->assets()
		->queue(MK_NAME);


    auto root   = scene::Node::create("root");
    auto camera	= scene::Node::create("camera");
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		scene::Node::Ptr mk = assets->node(MK_NAME);
		
		root->addComponent(sceneManager);

		// setup camera
		auto renderer = Renderer::create();
		renderer->backgroundColor(0x7F7F7FFF);
		camera->addComponent(renderer);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 20.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
		root->addChild(camera);

		baseNode->addComponent(Transform::create());

		//RandomScene(baseNode, 3, 2.f, assets);
		baseNode->addChild(mk);

		pointer->addComponent(Transform::create());
		pointer->component<Transform>()->transform()->prependScale(0.3f, 0.3f, 0.3f)->appendTranslation(0, 0, 5.0f);
		pointer->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f)),
			assets->effect("effect/Basic.effect")
		));

		root->addChild(baseNode);
		root->addChild(pointer);
        
        //getBoundingBoxes(baseNode, *boxList);
	});
    
	sceneManager->assets()->load();

	Leap::Controller* controller = new Leap::Controller();
	controller->enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller->enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);


	Leap::Frame lastFrame;
	float angle = 0.f;
	float targetAngle = 0.f;
	bool inProgress = false;
	int totalMoveTime = 0;
	int explodeThreshold = 7;
	bool exploded = false;
	float currentExplodeValue = 0.f;


	Vector3::Ptr targetPos = Vector3::create();
	float lastgap = 0.0f;
	const float delta = 5.f;

	bool done = false;
	while (!done)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			default:
				break;
			}
		}

		Leap::Frame frame = controller->frame();

		const Leap::GestureList gestures = frame.gestures();

		const Leap::HandList hands = frame.hands();

		

		if (hands.count() >= 2)
		{
			Leap::Hand lhand = hands.leftmost();
			Leap::Hand rhand = hands.rightmost();

			float gap = (rhand.palmPosition() - lhand.palmPosition()).magnitude();;

			if (lhand.palmNormal().x > 0.5f && rhand.palmNormal().x < -0.5f)
			{
				if (gap > lastgap + delta)
				{
					totalMoveTime += 7;
					//scaleSpeed = scaleSpeed + (1.5f - scaleSpeed) * 0.01f;
				}
				else if (gap < lastgap - delta)
				{
					totalMoveTime -= 7;
					//scaleSpeed = scaleSpeed + (0.5f - scaleSpeed) * 0.01f;
				}
					

				if (totalMoveTime >= explodeThreshold)
				{
					exploded = true;
					totalMoveTime = 0;
					std::cout << "explode" << std::endl;
				}
				else if (totalMoveTime <= -explodeThreshold)
				{
					exploded = false;
					totalMoveTime = 0;
					std::cout << "unexplode" << std::endl;
				}
			}
			lastgap = gap;

			totalMoveTime -= 1 * ((0 < totalMoveTime) - (0 > totalMoveTime));
			std::cout << totalMoveTime << ":" << gap << std::endl;
		}
		else
		{
			for (int g = 0; g < gestures.count(); g++)
			{
				Leap::Gesture gesture = gestures[g];
				switch (gesture.type())
				{
				case Leap::Gesture::TYPE_SWIPE:
					{
						Leap::SwipeGesture swipe = gesture;
						if (gesture.state() == Leap::Gesture::State::STATE_START && !inProgress)
						{
							inProgress = true;
							if (abs(angle - targetAngle) < PI / 10.f);
								targetAngle += PI / 2.f * ((0.f < swipe.direction().x) - (swipe.direction().x < 0.f));
						}
						if (gesture.state() == Leap::Gesture::State::STATE_STOP)
							inProgress = false;
						break;
					}
				case Leap::Gesture::TYPE_SCREEN_TAP:
					{
						pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
                        Leap::ScreenTapGesture screenTape = gesture;
                        
                        std::cout << screenTape.direction().x << ":"<< screenTape.direction().y <<":"<< screenTape.direction().z << std::endl;
                        std::cout << screenTape.position().x << ":"<< screenTape.position().y <<":"<< screenTape.position().z << std::endl;
                        
                        auto touche = getTouchedMesh(camera,
                                                     pointer,
                                                     *boxList);
                        if (touche)
                        touche->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
                        break;
					}
				default:
					break;
				}
				
			}

			Leap::PointableList pointables = frame.pointables();
			//if (pointables.count() >=4)
			//	speed = 0.f;
			if (pointables.count() >= 1)
			{
				auto finger = pointables[0];
				auto tip = finger.tipPosition() * 0.1f;
				targetPos->lerp(Vector3::create(tip.x, tip.y - 30.f, 15.f), 0.05f);
			}
		}

		angle = angle + (targetAngle - angle) * 0.01f;

		float explodeTarget = exploded ? 2.f : 0.f;

		float explodeDelta = currentExplodeValue + (explodeTarget - currentExplodeValue) * 0.01f;
		explode(baseNode, explodeDelta - currentExplodeValue);
		currentExplodeValue = explodeDelta;

		selectedMesh->component<Transform>()->transform()->identity()->prependRotationY(angle);
		pointer->component<Transform>()->transform()->identity()->appendScale(0.3f, 0.3f, 0.3f)->prependTranslation(targetPos);
		//scaleSpeed = scaleSpeed + (1.f - scaleSpeed) * 0.1f;
		//std::cout << scaleSpeed << std::endl;

		sceneManager->nextFrame();

		pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f));
#ifdef MINKO_ANGLE
		eglSwapBuffers(context->eglDisplay, context->eglSurface);
#elif defined(EMSCRIPTEN)
		SDL_GL_SwapBuffers();
#else
		SDL_GL_SwapWindow(window);
#endif
	}

	SDL_Quit();

	exit(EXIT_SUCCESS);
}
