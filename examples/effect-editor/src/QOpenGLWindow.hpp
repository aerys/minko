#pragma once

#include <QtGui/QWindow>
#include <QtGui/QOpenGLPaintDevice>

#include <minko/Minko.hpp>


class QOpenGLWindow :
	public QWindow
{
	Q_OBJECT

	private:
		typedef std::shared_ptr<minko::render::AbstractContext> AbstractContextPtr;
		typedef std::shared_ptr<minko::component::Renderer>		RendererPtr;
		typedef std::shared_ptr<minko::component::SceneManager>	SceneManagerPtr;

	private:
		QOpenGLPaintDevice*	_qdevice;
		QOpenGLContext*		_qcontext;
		AbstractContextPtr	_context;
		RendererPtr			_renderer;
		SceneManagerPtr		_sceneManager;

	public:
		explicit 
		QOpenGLWindow(QWindow* parent = nullptr);
		
		~QOpenGLWindow();

		virtual 
		void 
		render(QPainter*);
		
		virtual
		void
		render();

		virtual 
		void 
		initialize();

	public slots:
		void
		renderNow();

	protected:
		bool 
		event(QEvent*);

		void 
		exposeEvent(QExposeEvent*);

	private:
		void
		initializeWindow();

		void
		initializeScene();
};
