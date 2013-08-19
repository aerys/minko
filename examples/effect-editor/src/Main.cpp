#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtWidgets/QApplication>

#include "QMinkoGLWidget.hpp"
#include "QMinkoEffectEditor.hpp"
#include "QMinkoBindingsWidget.hpp"

int 
main(int argc, char **argv)
{
	Q_INIT_RESOURCE(QMinkoEffectEditor);

	QApplication app(argc, argv);
	QMinkoEffectEditor window;
	window.show();	

	return app.exec();
}