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
		QWidget					*_qTabWidget	[NUM_TABS];
		QWebFrame				*_qTabFrames	[NUM_TABS];
		QObject					*_qTabJSObjects	[NUM_TABS];
		QString					_qTabSources	[NUM_TABS];
		QSignalMapper			*_qAddEditorSignalMapper;

		QIcon					*_qIconSave, *_qIconSaveNeeded;
		bool					_saveNeeded;

		minko::Signal<minko::file::AbstractParser::Ptr>::Slot	_effectParserCompleteSlot;

	public:
	    explicit 
		QMinkoEffectEditor(QWidget *parent = 0);

	    ~QMinkoEffectEditor();

	public slots:
		void
		updateSource(int tabIndex);

	private slots:
		void
		updateEffectName();

		void
		exposeQObjectsToJS(int tabIndex);

		void
		loadMk();

		void
		loadEffect();

		void
		saveEffect();

	protected:
		void 
		resizeEvent(QResizeEvent);

	private:
		DISALLOW_COPY_AND_ASSIGN(QMinkoEffectEditor);

		void
		setupSourceTabs();

		void
		setupBindingsButtons();

		void
		loadMk(const QString&);

		void
		loadEffect(const QString&);

		void
		effectParserCompleteHandler(minko::file::AbstractParser::Ptr);
		
		static
		void 
		escapeSpecialCharacters(const std::string&, std::string&);

		static
		unsigned int countLeftmostExtraTabs(const std::string&);

		static
		void 
		removeLeftmostExtraTabs(const std::string&, std::string&);

		static
		void
		fix(const std::string&, std::string&);

		void
		saveEffect(const QString&);

		void
		createEffect(std::string&) const;

		void
		displayEffect() const;

		void
		tabModified(int tabIndex, bool);

		void
		saveNeeded(bool);
};
