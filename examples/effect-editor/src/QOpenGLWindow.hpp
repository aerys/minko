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
		bool				_initialized;
		bool				_animating;
		bool				_updatePending;

		std::shared_ptr<minko::scene::Node>	_box;

	public:
		explicit 
		QOpenGLWindow(QWindow* parent = nullptr);
		
		~QOpenGLWindow();

		/*
		virtual 
		void 
		render(QPainter*);
		
		virtual
		void
		render();
		*/

		virtual 
		void 
		initialize();

		inline
		bool
		animating() const
		{
			return _animating;
		}

		void
		setAnimating(bool);

	public slots:
		void
		renderNow();

		void
		renderLater();

	protected:
		bool 
		event(QEvent*);

		void 
		exposeEvent(QExposeEvent*);

		void
		resizeEvent(QResizeEvent*);

	private:
		void
		initializeWindow();

		void
		initializeScene();
};
