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
		QSignalMapper			*_qAddEditorSignalMapper;

	public:
	    explicit 
		QMinkoEffectEditor(QWidget *parent = 0);

	    ~QMinkoEffectEditor();

	public slots:
		void
		sourcesChangedSlot();

	private slots:
		void
		addEditorToJavaScript(int tabIndex);

		void
		loadMk();

		void
		loadEffect();

		void
		saveEffect();

	private:
		DISALLOW_COPY_AND_ASSIGN(QMinkoEffectEditor);

		void
		setupSourceTabs();

		void
		setupToolButtons();

		void
		loadMk(const QString&);

		void
		loadEffect(const QString&);

		void
		saveEffect(const QString&);
};
