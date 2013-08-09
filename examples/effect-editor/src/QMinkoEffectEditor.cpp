#include "QMinkoEffectEditor.hpp"

#include "ui/ui_QMinkoEffectEditor.h"
#include <QtWidgets/QToolButton>
#include <QtWidgets/QFileDialog>
#include <QtWebKitWidgets/QWebFrame>

QMinkoEffectEditor::QMinkoEffectEditor(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::QMinkoEffectEditor),
	_qAddEditorSignalMapper(new QSignalMapper(this))
{
    _ui->setupUi(this);

	setupToolButtons();

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
QMinkoEffectEditor::setupToolButtons()
{
	_ui->loadMkToolButton->setIcon(QIcon(":/resources/icon-load-mk.png"));
	_ui->loadEffectToolButton->setIcon(QIcon(":/resources/icon-load-effect.png"));
	_ui->saveEffectToolButton->setIcon(QIcon(":/resources/icon-save-effect.png"));

	QObject::connect(_ui->loadMkToolButton,		SIGNAL(clicked()),	this,	SLOT(loadMk()));
	QObject::connect(_ui->loadEffectToolButton,	SIGNAL(clicked()),	this,	SLOT(loadEffect()));
	QObject::connect(_ui->saveEffectToolButton,	SIGNAL(clicked()),	this,	SLOT(saveEffect()));
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
			_qAddEditorSignalMapper,
			SLOT(map())
		);

		_qAddEditorSignalMapper->setMapping(_qTabFrames[tabIndex], (int)tabIndex);

		QObject::connect(
			_qAddEditorSignalMapper, 
			SIGNAL(mapped(int)),
			this,
			SLOT(addEditorToJavaScript(int))
		);
	}
}

void
QMinkoEffectEditor::loadMk(const QString& filename)
{
	std::cout << "load mk file: " << qPrintable(filename) << std::endl;
}

void
QMinkoEffectEditor::loadEffect(const QString& filename)
{
	std::cout << "load effect file: " << qPrintable(filename) << std::endl;
}

void
QMinkoEffectEditor::saveEffect(const QString& filename)
{
	std::cout << "save effect file: " << qPrintable(filename) << std::endl;
}

void
QMinkoEffectEditor::addEditorToJavaScript(int tabIndex)
{
	if (tabIndex < 0 || tabIndex >= NUM_TABS)
		throw std::invalid_argument("tabIndex");

	_qTabFrames[tabIndex]->addToJavaScriptWindowObject("qMinkoEffectEditor", this);
}

/*slot*/
void
QMinkoEffectEditor::sourcesChangedSlot()
{

	std::cout << "sources changed" << std::endl;
	QVariant qVariant = _qTabFrames[TAB_VERTEX_SOURCE]->evaluateJavaScript("codeMirror.getValue()");
}

/*slot*/
void
QMinkoEffectEditor::loadMk()
{
	const QString& filename = QFileDialog::getOpenFileName(
		this,
		"Load *.mk file",
		QString(),
		"MK files (*.mk)"
	);
	if (filename.isEmpty())
		return;

	loadMk(filename);
}

/*slot*/
void
QMinkoEffectEditor::loadEffect()
{
	const QString& filename = QFileDialog::getOpenFileName(
		this,
		"Load *.effect file",
		QString(),
		"Effect files (*.effect)"
	);
	if (filename.isEmpty())
		return;

	loadEffect(filename);
}

/*slot*/
void
QMinkoEffectEditor::saveEffect()
{
	const QString& filename = QFileDialog::getSaveFileName(
		this,
		"Save *.effect file",
		QString(),
		"Effect files (*.effect)"
	);
	if (filename.isEmpty())
		return;

	saveEffect(filename);
}