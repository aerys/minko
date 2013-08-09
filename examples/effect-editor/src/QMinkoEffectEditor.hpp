#pragma once

#include <QtCore/QSignalMapper>
#include <QtWidgets/QMainWindow>
#include <QtWebKitWidgets/QWebFrame>

#include "QMinkoGLWidget.hpp"

namespace Ui 
{
	class QMinkoEffectEditor;
}

class QMinkoEffectEditor : 
	public QMainWindow
{
    Q_OBJECT
    
	public:
		typedef enum
		{
			TAB_VERTEX_SOURCE = 0,
			TAB_FRAGMENT_SOURCE,
			TAB_BINDINGS,
			NUM_TABS
		}
		TabType;

	private:
		Ui::QMinkoEffectEditor	*_ui;
		QWebFrame				*_qTabFrames[NUM_TABS];
		QSignalMapper			*_qSignalMapper;

	public:
	    explicit 
		QMinkoEffectEditor(QWidget *parent = 0);

	    ~QMinkoEffectEditor();

		void
		setupSourceTabs();

	public slots:
		void
		sourcesSavedSlot();

	private slots:
		void
		addEditorToJavaScript(int tabIndex);

	private:
		DISALLOW_COPY_AND_ASSIGN(QMinkoEffectEditor);
};
