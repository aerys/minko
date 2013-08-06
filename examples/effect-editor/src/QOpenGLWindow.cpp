#include "QOpenGLWindow.hpp"

#include <QtGui/QPainter>

#include <minko/render/OpenGLES2Context.hpp>

using namespace minko;
using namespace minko::render;
using namespace minko::component;

/*explicit*/
QOpenGLWindow::QOpenGLWindow(QWindow* parent):
	QWindow(parent),
	_qdevice(nullptr),
	_qcontext(nullptr),
	_context(nullptr),
	_renderer(nullptr),
	_sceneManager(nullptr)
{
	setSurfaceType(QWindow::OpenGLSurface);
}

QOpenGLWindow::~QOpenGLWindow()
{
	if (_qdevice)
		delete _qdevice;
}

void
QOpenGLWindow::initializeWindow()
{
	if (_qdevice == nullptr)
		_qdevice	= new QOpenGLPaintDevice;

	if (_qcontext == nullptr)
	{
		_qcontext	= new QOpenGLContext(this);
		_qcontext->setFormat(requestedFormat());
		_qcontext->create();
	}
	_qcontext->makeCurrent(this);

	if (_context == nullptr)
	{
		_context	= OpenGLES2Context::create(); 

#ifdef DEBUG
	std::cout << "OpenGL context created\n\t" << _context->driverInfo() << std::endl;
#endif // DEBUG
	}
	
}

void
QOpenGLWindow::initializeScene()
{
	if (_context == nullptr)
		initializeWindow();

	_sceneManager	= SceneManager::create(_context);
	_renderer		= Renderer::create();


}

/*virtual*/
void
QOpenGLWindow::render()
{
	if (_qdevice == nullptr)
		initializeWindow();

	_qdevice->setSize(size());

	QPainter painter(_qdevice);
	render(&painter);
}

/*virtual*/
void 
QOpenGLWindow::render(QPainter*)
{

}

/*virtual*/
void
QOpenGLWindow::initialize()
{
}

void 
QOpenGLWindow::exposeEvent(QExposeEvent*)
{
    if (isExposed())
        renderNow();
}

bool 
QOpenGLWindow::event(QEvent* event)
{
    switch (event->type()) 
	{
    case QEvent::UpdateRequest:
        //m_update_pending = false;
        renderNow();
		return true;

    default:
        return QWindow::event(event);
    }
}

void
QOpenGLWindow::renderNow()
{
	if (_renderer == nullptr)
		initializeScene();

	_renderer->render();

	//_sceneManager->nextFrame();
}