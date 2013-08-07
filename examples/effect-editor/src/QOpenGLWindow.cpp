#include "QOpenGLWindow.hpp"

#include <QtCore/QCoreApplication>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QPaintEvent>

#include <minko/MinkoPNG.hpp>
#include <minko/math/Vector4.hpp>
#include <minko/render/OpenGLES2Context.hpp>


using namespace minko;

/*explicit*/
QOpenGLWindow::QOpenGLWindow(QWindow* parent):
	QWindow(parent),
	_qdevice(nullptr),
	_qcontext(nullptr),
	_context(nullptr),
	_renderer(nullptr),
	_sceneManager(nullptr),
	_initialized(false),
	_animating(false),
	_updatePending(false),
	_box(nullptr)
{
	setSurfaceType(QWindow::OpenGLSurface);
}

QOpenGLWindow::~QOpenGLWindow()
{
	if (_qdevice)
		delete _qdevice;
}

/*virtual*/
void
QOpenGLWindow::initialize()
{
#ifdef DEBUG_QGLWINDOW
	std::cout << "QOpenGLWindow initialize" << std::endl;
#endif // DEBUG_QGLWINDOW

	initializeWindow();
	initializeScene();
}

void
QOpenGLWindow::initializeWindow()
{
	if (_qdevice == nullptr)
	{
		_qdevice	= new QOpenGLPaintDevice;
	}
	_qdevice->setSize(size());

	if (_qcontext == nullptr)
	{
		_qcontext	= new QOpenGLContext(this);
		_qcontext->setFormat(requestedFormat());
		_qcontext->create();
	}
	_qcontext->makeCurrent(this);

	if (_context == nullptr)
	{
		_context	= render::OpenGLES2Context::create(); 
#ifdef DEBUG_QGLWINDOW
	std::cout << "OpenGL context created\n\t" << _context->driverInfo() << std::endl;
#endif // DEBUG_QGLWINDOW
	}
}

void
QOpenGLWindow::initializeScene()
{
	if (_context == nullptr)
		initializeWindow();

	_sceneManager = component::SceneManager::create(_context);
	_sceneManager->assets()->defaultOptions()->includePaths().insert("assets");
	_sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(_context))
		->queue("texture/box.png")
		->queue("effect/Basic.effect");

	_renderer	= component::Renderer::create();
	_renderer->backgroundColor(0x7F7F7FFF);

	_box		= scene::Node::create("box");

	auto _ = _sceneManager->assets()->complete()->connect( [=](file::AssetLibrary::Ptr assets)
	{
		auto root	= scene::Node::create("root");
		auto camera	= scene::Node::create("camera");

		root->addComponent(_sceneManager);

		camera->addComponent(_renderer);
		camera->addComponent(component::Transform::create());
		camera->component<component::Transform>()->transform()
			->lookAt(math::Vector3::zero(), math::Vector3::create(0.0f, 0.0f, 3.0f), math::Vector3::yAxis());

		camera->addComponent(component::PerspectiveCamera::create(
			PI * 0.25f, 
			width() / (float)height(), 
			0.1f, 
			1000.0f
		));
		root->addChild(camera);

		_box->addComponent(component::Transform::create());
		_box->addComponent(component::Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor", math::Vector4::create(1.0f, 1.0f, 1.0f, 1.0f))
				->set("material.diffuseMap", assets->texture("texture/box.png")),
			assets->effect("effect/Basic.effect")
		));
		root->addChild(_box);

		_initialized = true;
	});

	_sceneManager->assets()->load();
}

/*virtual*/
/*
void
QOpenGLWindow::render()
{
#ifdef DEBUG_QGLWINDOW
	std::cout << "QOpenGLWindow::render" << std::endl;
#endif // DEBUG_QGLWINDOW

	if (_qdevice == nullptr)
		initializeWindow();

	_qdevice->setSize(size());

	QPainter painter(_qdevice);
	render(&painter);
}
*/

/*virtual*/
/*
void 
QOpenGLWindow::render(QPainter*)
{
#ifdef DEBUG_QGLWINDOW
	std::cout << "QOpenGLWindow::render(QPainter*)" << std::endl;
#endif // DEBUG_QGLWINDOW
}
*/

void 
QOpenGLWindow::exposeEvent(QExposeEvent*)
{
	/*
#ifdef DEBUG_QGLWINDOW
	std::cout << "QOpenGLWindow::exposeEvent\texposed ? " << isExposed() << std::endl;
#endif // DEBUG_QGLWINDOW
*/
    if (isExposed())
		//update();
        renderNow();
}

void
QOpenGLWindow::resizeEvent(QResizeEvent* e)
{
	std::cout << "resize event" << e->size().width() << " x " << e->size().height() << std::endl;

	if (_context == nullptr)
		return;

	const float retinaScale = devicePixelRatio();
	_context->configureViewport(0, 0, width()*retinaScale, height()*retinaScale);
}


bool 
QOpenGLWindow::event(QEvent* event)
{
    switch (event->type()) 
	{
    case QEvent::UpdateRequest:

		/*
#ifdef DEBUG_QGLWINDOW
	std::cout << "QOpenGLWindow::event\tupdate request" << std::endl;
#endif // DEBUG_QGLWINDOW
	*/
		_updatePending = false;
        renderNow();
		return true;

    default:
        return QWindow::event(event);
    }
}

void
QOpenGLWindow::renderNow()
{
	/*
#ifdef DEBUG_QGLWINDOW
	std::cout << "QOpenGLWindow::renderNow" << std::endl;
#endif // DEBUG_QGLWINDOW
	*/
	if (!isExposed())
		return;

	if (!_initialized)
		initialize();

	//render();

	_renderer->render();
	_sceneManager->nextFrame();

	_qcontext->swapBuffers(this);

	if (_animating)
		renderLater();
}

void
QOpenGLWindow::renderLater()
{
	if (!_updatePending) 
	{
        _updatePending = true;

		if (_box)
		_box->component<component::Transform>()->transform()
			->appendRotationY(0.01f);

        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

void
QOpenGLWindow::setAnimating(bool value)
{
	_animating = value;
	if (_animating)
		renderLater();
}