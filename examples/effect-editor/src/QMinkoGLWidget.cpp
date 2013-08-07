#include "QMinkoGLWidget.hpp"

#include <minko/render/OpenGLES2Context.hpp>
#include <minko/math/Vector4.hpp>
#include <minko/MinkoPNG.hpp>

using namespace minko;

void
QMinkoGLWidget::initializeGL()
{
#ifdef DEBUG_QGLWINDOW
	std::cout << "QMinkoGLWidget initialize" << std::endl;
#endif // DEBUG_QGLWINDOW

	initializeMinkoContext();
	initializeMinkoScene();
}

void
QMinkoGLWidget::paintGL()
{
	if (!_initialized)
		return;

	_model->component<component::Transform>()->transform()
		->prependRotationY(0.02f);

	_renderer->render();
	_sceneManager->nextFrame();
}

void
QMinkoGLWidget::resizeGL(int width, int height)
{
	if (!_initialized)
		return;

	_context->configureViewport(0, 0, width, height);
	//_camera->component<component::PerspectiveCamera>();
}

void
QMinkoGLWidget::initializeMinkoContext()
{
	if (_context)
		return;

	_context = render::OpenGLES2Context::create(); 
#ifdef DEBUG_QGLWINDOW
	std::cout << "OpenGL minko context created\n\t" << _context->driverInfo() << std::endl;
#endif // DEBUG_QGLWINDOW
}

void
QMinkoGLWidget::initializeMinkoScene()
{
	if (!_context)
		initializeMinkoContext();

	_sceneManager = component::SceneManager::create(_context);
	_sceneManager->assets()->defaultOptions()->includePaths().insert("assets");
	_sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(_context))
		->queue("texture/box.png")
		->queue("effect/Basic.effect");

	_renderer	= component::Renderer::create();
	_renderer->backgroundColor(0x7F7F7FFF);

	_model		= scene::Node::create("box");
	_camera		= scene::Node::create("camera");

	auto _ = _sceneManager->assets()->complete()->connect( [=](file::AssetLibrary::Ptr assets)
	{
		auto root	= scene::Node::create("root");

		root->addComponent(_sceneManager);

		_camera->addComponent(_renderer);
		_camera->addComponent(component::Transform::create());
		_camera->component<component::Transform>()->transform()
			->lookAt(math::Vector3::zero(), math::Vector3::create(0.0f, 0.0f, 3.0f), math::Vector3::yAxis());

		_camera->addComponent(component::PerspectiveCamera::create(
			PI * 0.25f, 
			width() / (float)height(), 
			0.1f, 
			1000.0f
		));
		root->addChild(_camera);

		_model->addComponent(component::Transform::create());
		_model->addComponent(component::Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor", math::Vector4::create(1.0f, 1.0f, 1.0f, 1.0f))
				->set("material.diffuseMap", assets->texture("texture/box.png")),
			assets->effect("effect/Basic.effect")
		));
		root->addChild(_model);

		_qtimer->start();

		_initialized = true;
	});

	_sceneManager->assets()->load();
}
