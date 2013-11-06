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
			if (magnitude > 0.f)
				cTransform->appendTranslation(direction * magnitude);
			else
				cTransform->appendTranslation((direction * magnitude) * (1.f /(magnitude - 1.f)) * -1);
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

static const float cameraXAngle = 35.f * PI / 180.f;
static const float cameraYAngle = 45.f * PI / 180.f;  

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
        
        unsigned int vertexSize = provider->get<render::VertexBuffer::Ptr>("geometry.vertex.attribute.position")->vertexSize();
        for (int i = 0; i < vertices.size(); i +=vertexSize)
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

	int numObjects = rand() % 6;
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
getTouchedMesh(scene::Node::Ptr camera, scene::Node::Ptr pointer, std::vector<boundingBox>& boxList, math::Matrix4x4::Ptr worldToScreen)
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
        
        auto rayStart = modelCameraTransform->translation();
        auto pointerTranslation =modelPointerTransform->translation();
        
        auto direction = (modelPointerTransform->translation() - modelCameraTransform->translation())->normalize();
        
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
    
//    for (boundingBox box : boxList)
//    {
//        auto nodeTransform = Matrix4x4::create(box.node->component<Transform>()->transform());
//        auto modelToWorldMat = box.node->component<Transform>()->modelToWorldMatrix();
//        auto modelPointerTransform = Matrix4x4::create(pointer->component<Transform>()->transform());
//        
//        nodeTransform->append(modelToWorldMat);
//        nodeTransform->append(worldToScreen);
//        modelPointerTransform->append(pointer->component<Transform>()->modelToWorldMatrix());
//        modelPointerTransform->append(worldToScreen);
//        
//        auto corner1 = nodeTransform->transform(box.corner1);
//        auto corner2 = nodeTransform->transform(box.corner2);
//    
//        Vector3::Ptr nearCorner, farCorner;
//        if (corner1->x() > corner2->x())
//        {
//            nearCorner = corner1;
//            farCorner = corner2;
//        }
//        else
//        {
//            nearCorner = corner2;
//            farCorner = corner1;
//        }
//
//        if (modelPointerTransform->translation()->x() > farCorner->x()
//            && modelPointerTransform->translation()->x() < nearCorner->x())
//        {
//            if (modelPointerTransform->translation()->y() > farCorner->y()
//                && modelPointerTransform->translation()->y() < nearCorner->y())
//            {
//                touchedNodes.push_back(box.node);
//                result = box.node;
//            }
//        }
//    }
    
    //Get only the nearest mesh
    if (touchedNodes.size() > 0)
        result = touchedNodes[0];
    
    for (int i= 0; i < touchedNodes.size(); i++)
    {
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

	int windowWidth;
	int windowHeight;
	bool fullscreen = false;
	if (argc >= 2 && strcmp(argv[1], "--fullscreen") == 0)
		fullscreen = true;

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
		"Minko - Leap Motion Example",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 640,
		SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)
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

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());
	auto baseNode = scene::Node::create("baseNode");
	auto ambientLightNode	= scene::Node::create("ambientLight");
	auto pointer = scene::Node::create("pointer");
    auto skybox = scene::Node::create("skybox");
	auto selectedMesh = baseNode;
    auto pLight1 = scene::Node::create("pointLight1");
    auto pLight2 = scene::Node::create("pointLight2");
    
	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context()))
    ->geometry("skybox", geometry::SphereGeometry::create(sceneManager->assets()->context(), 80, 80, true))
        ->queue("texture/hangar.png");
  
#ifdef DEBUG
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif
    
	auto options = sceneManager->assets()->defaultOptions();
	auto defaultMaterial = material::Material::create()
		->set("diffuseColor", Vector4::create(1.f, 1.f, 1.f, 1.f))
		->set("triangleCulling", render::TriangleCulling::NONE);
    
	options->material(data::Provider::create()->set("material.triangleCulling", render::TriangleCulling::NONE));
	options->generateMipmaps(true);
    
    sceneManager->assets()
    ->load("effect/Phong.effect")
    ->load("effect/Basic.effect");
    options->materialFunction([&](const std::string&, data::Provider::Ptr){
		return defaultMaterial;
	});
	options->effect(sceneManager->assets()->effect("effect/Phong.effect"));
    
	sceneManager->assets()
    ->queue(MK_NAME);
    
    
    auto root   = scene::Node::create("root");
    auto camera	= scene::Node::create("camera");
	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		scene::Node::Ptr mk = assets->node(MK_NAME);
		
		root->addComponent(sceneManager);

		auto lights = scene::Node::create();

		lights
			//->addComponent(component::AmbientLight::create())
			->addComponent(component::DirectionalLight::create())
			->addComponent(component::Transform::create());
		lights->component<Transform>()->transform()->lookAt(Vector3::zero(), Vector3::create(-1.f, -1.f, -1.f));
		root->addChild(lights);

		// setup camera
		auto renderer = Renderer::create();
		renderer->backgroundColor(0x7F7F7FFF);
		camera->addComponent(renderer);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(20.f * sin(cameraYAngle), 20.f * sin(cameraXAngle) * cos(cameraXAngle), 20.f * cos(cameraXAngle)));
		camera->addComponent(PerspectiveCamera::create(windowWidth * 1.0f / windowHeight));
		root->addChild(camera);

		baseNode->addComponent(Transform::create());

		//RandomScene(baseNode, 3, 2.f, assets);
		baseNode->addChild(mk);

		pointer->addComponent(Transform::create());
		pointer->component<Transform>()->transform()->prependScale(0.3f, 0.3f, 0.3f)->appendTranslation(0, 0, -5.0f);
		pointer->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f)),
			assets->effect("effect/Phong.effect")
		));
        pLight1->addComponent(component::PointLight::create());
        pLight1->addComponent(component::Transform::create());
        pLight1->component<Transform>()->transform()->appendTranslation(0.f, 10.f, 0.f);
        pLight1->component<PointLight>()->color()->setTo(1.f, 1.f, 1.f);
		/*pLight1->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor", Vector4::create(1.f, 0.f, 0.f, 0.f)),
			assets->effect("effect/Phong.effect")
			));*/
        
        skybox->addComponent(Transform::create());
        skybox->component<Transform>()->transform()->prependScale(60.0f, 60.0f, 60.0f);
        skybox->addComponent(Surface::create(
             assets->geometry("skybox"),
             data::Provider::create()
             
             ->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 1.f))
             ->set("material.diffuseMap",	assets->texture("texture/hangar.png"))
             ->set("material.triangleCulling", render::TriangleCulling::FRONT),
             assets->effect("effect/Basic.effect")
                             ));
        
        root->addChild(skybox);
		root->addChild(baseNode);
		//camera->addChild(pointer);
        root->addChild(pLight1);
        
        getBoundingBoxes(baseNode, *boxList);
	});
    
	sceneManager->assets()->load();
    
    PerspectiveCamera::Ptr cam = camera->component<PerspectiveCamera>();
    
	Leap::Controller* controller = new Leap::Controller();
	controller->enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller->enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    
    
	Leap::Frame lastFrame;
	float angle = 0.f;
	float targetAngle = 0.f;
	float cameraDistance = 20.f;
	float targetcameraDistance = 20.f;
	bool inProgress = false;
	float totalMoveTime = 0;
	float explodeThreshold = 1.f;
	bool exploded = false;
	float currentExplodeValue = 0.f;
	bool zoomed = false;

    int fingerID = -1;
	Uint32 lastTicks = 0;
	float frameTime;
    
	Vector3::Ptr targetPos = Vector3::create();
	float lastgap = 0.0f;
	const float delta = 5.f;
    
	bool done = false;
	while (!done)
	{
		SDL_Event event;

		Uint32 ticks = SDL_GetTicks();
		frameTime = (ticks - lastTicks) / 1000.f;
		lastTicks = ticks;
        
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

		const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
		if (keyboardState[SDL_SCANCODE_ESCAPE])
			done = true;
        
		Leap::Frame frame = controller->frame();
        
		const Leap::GestureList gestures = frame.gestures();
        
		const Leap::HandList hands = frame.hands();
        
		
        
		if (hands.count() >= 2)
		{
			Leap::Hand lhand = hands.leftmost();
			Leap::Hand rhand = hands.rightmost();
            
			float gap = (rhand.palmPosition() - lhand.palmPosition()).magnitude();;
            if (lastgap < 0)
			{
				lastgap = gap;
			}
			if (gap > lastgap + delta)
			{
				totalMoveTime += 10 * frameTime;
				//scaleSpeed = scaleSpeed + (1.5f - scaleSpeed) * 0.01f;
			}
			else if (gap < lastgap - delta)
			{
				totalMoveTime -= 10 * frameTime;
				//scaleSpeed = scaleSpeed + (0.5f - scaleSpeed) * 0.01f;
			}

			if(!inProgress)
			if (totalMoveTime >= explodeThreshold)
			{
				inProgress = true;
				exploded = true;
				totalMoveTime = 0;
				std::cout << "explode" << std::endl;
			}
			else if (totalMoveTime <= -explodeThreshold)
			{
				inProgress = true;
				exploded = false;
				totalMoveTime = 0;
				std::cout << "unexplode" << std::endl;
			}
			lastgap = gap;
            
			totalMoveTime -= 0.5 * frameTime * ((0 < totalMoveTime) - (0 > totalMoveTime));
		}
		else
		{
			lastgap = -1;
			for (int g = 0; g < gestures.count(); g++)
			{
				Leap::Gesture gesture = gestures[g];
				switch (gesture.type())
				{
                    case Leap::Gesture::TYPE_SWIPE:
					{
						Leap::SwipeGesture swipe = gesture;
						if (fabs((swipe.direction().x)) > 0.5f)
						{
							if (gesture.state() == Leap::Gesture::State::STATE_START && !inProgress)
							{
								inProgress = true;
								if (fabs((angle - targetAngle)) < PI / 10.f);
								targetAngle += PI / 2.f * ((0.f < swipe.direction().x) - (swipe.direction().x < 0.f));
							}
						}
						else
						{
							if (fabs(swipe.direction().y) > 0.7)
								zoomed = swipe.direction().y < 0;
						}
						break;
					}
                    case Leap::Gesture::TYPE_SCREEN_TAP:
					{
						//pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
                        //Leap::ScreenTapGesture screenTape = gesture;
                        
                        //std::cout << screenTape.direction().x << ":"<< screenTape.direction().y <<":"<< screenTape.direction().z << std::endl;
                        //std::cout << screenTape.position().x << ":"<< screenTape.position().y <<":"<< screenTape.position().z << std::endl;
                        
                        /*auto touche = getTouchedMesh(camera,
                                                     pointer,
                                                     *boxList,
                                                     worldToScreen);
                        if (touche)
                            touche->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));*/
                        break;
					}
                    default:
                        break;
				}
				
			}

			Leap::PointableList pointables = frame.pointables();
			Leap::Pointable finger = frame.finger(fingerID);
			if (!finger.isValid())
			{
				if (pointables.count() >= 1)
				{
					finger = pointables[0];
					fingerID = finger.id();
				}
			}
			if (finger.isValid())
			{
				auto tip = finger.tipPosition() * 0.05f;
				targetPos->lerp(Vector3::create(tip.x, tip.y, -15.f), frameTime * 2);
			}
			else
				fingerID = -1;
		}

		float explodeTarget = exploded ? 1.5f : 0.f;
        if (fabs((targetAngle - angle)) > PI / 2 * 0.01f)
		{
			angle = angle + (targetAngle - angle) * frameTime * 2;
		}
		
		if (fabs((targetAngle - angle)) < PI / 2 * 0.1f
			&& fabs((explodeTarget - currentExplodeValue)) < 0.05f)
				inProgress = false;

		
		float explodeDelta = currentExplodeValue + (explodeTarget - currentExplodeValue) * frameTime * 2;
		explode(baseNode, explodeDelta - currentExplodeValue);
		currentExplodeValue = explodeDelta;

		targetcameraDistance = zoomed ? 10.f : 20.f;
		cameraDistance = cameraDistance + (targetcameraDistance - cameraDistance) * frameTime * 2;
        camera->component<Transform>()->transform()->identity()
			->lookAt(Vector3::zero(), Vector3::create(cameraDistance * sin(cameraYAngle + angle),
													  cameraDistance * sin(cameraXAngle) * cos(cameraYAngle),
													  cameraDistance * cos(cameraXAngle + angle)));
		
		//pointer->component<Transform>()->transform()->identity()->appendScale(0.1f, 0.1f, 0.1f)->prependTranslation(targetPos);
		//scaleSpeed = scaleSpeed + (1.f - scaleSpeed) * 0.1f;
		//std::cout << scaleSpeed << std::endl;
        
		sceneManager->nextFrame();
        
		//pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f));
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
