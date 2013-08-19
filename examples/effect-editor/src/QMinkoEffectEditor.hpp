#pragma once

#include <QtCore/QSignalMapper>
#include <QtWidgets/QMainWindow>
#include <QtWebKitWidgets/QWebFrame>

#include <json/json.h>

#include <minko/Minko.hpp>
#include "minko/render/BindingType.hpp"

#include "EditorCommon.hpp"

namespace Ui 
{
	class QMinkoEffectEditor;
}

class QMinkoEffectEditor : 
	public QMainWindow
{
    Q_OBJECT
    
	private:
		Ui::QMinkoEffectEditor	*_ui;

		QWebFrame				*_qVertexWebFrame;
		QObject					*_qVertexObjectJS;
		QString					_qVertexShaderSource;

		QWebFrame				*_qFragmentWebFrame;
		QObject					*_qFragmentObjectJS;
		QString					_qFragmentShaderSource;

		QString					_qBindingsSource;

		QIcon					*_qIconSave;
		QIcon					*_qIconSaveNeeded;
		bool					_saveNeeded;

		Json::Value				_effectRoot;

		minko::Signal<minko::file::AbstractParser::Ptr>::Slot	_effectParserCompleteSlot;

	public:
	    explicit 
		QMinkoEffectEditor(QWidget *parent = 0);

	    ~QMinkoEffectEditor();

	public slots:
		void
		updateSource(const QString&);

	private slots:
		void
		updateEffectName();

		void
		updateBindings(minko::render::BindingType);

		void
		exposeQObjectsToVertexJS();

		void
		exposeQObjectsToFragmentJS();

		void
		loadMk();

		void
		loadEffect();

		void
		saveEffect();

		void
		previewEffectFile();

	protected:
		void 
		resizeEvent(QResizeEvent);

	private:
		DISALLOW_COPY_AND_ASSIGN(QMinkoEffectEditor);

		void
		setupSourceTabs();

		void
		setupListTabs();

		void
		setupIOButtons();

		void
		loadMk(const QString&);

		void
		loadEffect(const QString&);

		void
		effectParserCompleteHandler(minko::file::AbstractParser::Ptr);
	
		void
		saveEffect(const QString&);

		void
		createEffect();

		void
		displayEffect();

		void
		saveNeeded(bool);

		static
		void
		restoreSpecialCharacters(const std::string&, std::string&);

		static
		void 
		escapeSpecialCharacters(const std::string&, std::string&);

		static
		unsigned int 
		countLeftmostExtraTabs(const std::string&);

		static
		void 
		removeLeftmostExtraTabs(const std::string&, std::string&);

		static
		void
		fix(const std::string&, std::string&);
};
