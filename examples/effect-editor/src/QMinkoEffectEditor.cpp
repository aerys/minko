#include "QMinkoEffectEditor.hpp"

#include "ui/ui_QMinkoEffectEditor.h"
#include <QtWebKitWidgets/QWebFrame>

QMinkoEffectEditor::QMinkoEffectEditor(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::QMinkoEffectEditor),
	_qSignalMapper(new QSignalMapper(this))
{
    _ui->setupUi(this);

	_qTabFrames[TAB_VERTEX_SOURCE]		= _ui->vertexWebView->page()->mainFrame();
	_qTabFrames[TAB_FRAGMENT_SOURCE]	= _ui->fragmentWebView->page()->mainFrame();
	_qTabFrames[TAB_BINDINGS]			= _ui->bindingsWebView->page()->mainFrame();



	setupSourceTabs();
}

QMinkoEffectEditor::~QMinkoEffectEditor()
{
    delete _ui;
}

void
QMinkoEffectEditor::setupSourceTabs()
{
	for (int tabIndex = TAB_VERTEX_SOURCE; tabIndex < NUM_TABS; ++tabIndex)
	{
		_qTabFrames[tabIndex]->load(QUrl("qrc:///resources/minimal-codemirror.html"));

		QObject::connect(
			_qTabFrames[tabIndex], 
			SIGNAL(javaScriptWindowObjectCleared()),
			_qSignalMapper,
			SLOT(map())
		);

		_qSignalMapper->setMapping(
			_qTabFrames[tabIndex], 
			(int)tabIndex
		);

		QObject::connect(
			_qSignalMapper, 
			SIGNAL(mapped(int)),
			this,
			SLOT(addEditorToJavaScript(int))
		);
	}
}

/*slot*/
void
QMinkoEffectEditor::addEditorToJavaScript(int tabIndex)
{
	if (tabIndex < 0 || tabIndex >= NUM_TABS)
		throw std::invalid_argument("tabIndex");

	_qTabFrames[tabIndex]->addToJavaScriptWindowObject("qMinkoEffectEditor", this);
}

/*slot*/
void
QMinkoEffectEditor::sourcesSavedSlot()
{
	std::cout << "sources changed" << std::endl;
}