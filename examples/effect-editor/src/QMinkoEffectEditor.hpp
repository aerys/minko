#pragma once

#include <QtWidgets/QMainWindow>

#include "QMinkoGLWidget.hpp"

namespace Ui 
{
	class QMinkoEffectEditor;
}

class QMinkoEffectEditor : 
	public QMainWindow
{
    Q_OBJECT
    
	private:
		Ui::QMinkoEffectEditor	*ui;

	public:
	    explicit QMinkoEffectEditor(QWidget *parent = 0);
	    ~QMinkoEffectEditor();
};
