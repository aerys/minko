#pragma once

#include <QtOpenGL/QGLWidget>
#include <QtCore/QTimer>

#include <minko/Minko.hpp>

class QMinkoGLWidget :
	public QGLWidget
{
	Q_OBJECT

	private:
		typedef std::shared_ptr<minko::render::AbstractContext> AbstractContextPtr;
		typedef std::shared_ptr<minko::component::Renderer>		RendererPtr;
		typedef std::shared_ptr<minko::component::SceneManager>	SceneManagerPtr;
		typedef std::shared_ptr<minko::scene::Node>				NodePtr;

	private:
		AbstractContextPtr	_context;
		RendererPtr			_renderer;
		SceneManagerPtr		_sceneManager;
		NodePtr				_camera;
		NodePtr				_model;
		bool				_initialized;
		bool				_animating;
		bool				_updatePending;

		QTimer				*_qtimer;

	public: 
		QMinkoGLWidget(QWidget * parent = nullptr, 
					  const QGLWidget * shareWidget = nullptr, 
					  Qt::WindowFlags f = 0) :
			QGLWidget(parent, shareWidget, f),
			_context(nullptr),
			_renderer(nullptr),
			_sceneManager(nullptr),
			_camera(nullptr),
			_initialized(false),
			_animating(false),
			_updatePending(false),
			_model(nullptr),
			_qtimer(new QTimer(this))
		{
			QObject::connect(_qtimer, SIGNAL(timeout()), this, SLOT(updateGL()));
		}
		
		~QMinkoGLWidget()
		{
		}

	protected:
		void 
		initializeGL();

		void
		paintGL();

		void
		resizeGL(int, int);

	private:
		void
		initializeMinkoContext();

		void
		initializeMinkoScene();
};
