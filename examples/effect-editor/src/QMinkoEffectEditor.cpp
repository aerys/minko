#include "QMinkoEffectEditor.hpp"

#include "ui/ui_QMinkoEffectEditor.h"
#include <QtWidgets/QToolButton>
#include <QtWidgets/QFileDialog>
#include <QtWebKitWidgets/QWebFrame>

QMinkoEffectEditor::QMinkoEffectEditor(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::QMinkoEffectEditor),
	_qAddEditorSignalMapper(new QSignalMapper(this)),
	_saveNeeded(false)
{
    _ui->setupUi(this);

	QObject::connect(_ui->effectNameLineEdit, SIGNAL(editingFinished()), this, SLOT(updateEffectName()));

	setupToolButtons();

	_qTabWidget[TAB_VERTEX_SOURCE]		= _ui->vertexTab;
	_qTabWidget[TAB_FRAGMENT_SOURCE]	= _ui->fragmentTab;
	_qTabWidget[TAB_BINDINGS]			= _ui->bindingsTab;

	_qTabFrames[TAB_VERTEX_SOURCE]		= _ui->vertexWebView->page()->mainFrame();
	_qTabFrames[TAB_FRAGMENT_SOURCE]	= _ui->fragmentWebView->page()->mainFrame();
	_qTabFrames[TAB_BINDINGS]			= _ui->bindingsWebView->page()->mainFrame();

	for (int tabIndex = TAB_VERTEX_SOURCE; tabIndex < NUM_TABS; ++tabIndex)
	{
		_qTabSources[tabIndex].clear();

		_qTabJSObjects[tabIndex] = new QObject(this);
		_qTabJSObjects[tabIndex]->setProperty("tabIndex", QVariant(tabIndex));
	}

	setupSourceTabs();
}

QMinkoEffectEditor::~QMinkoEffectEditor()
{
    delete _ui;
	if (_qIconSave)
		delete _qIconSave;
	if (_qIconSaveNeeded)
		delete _qIconSaveNeeded;
}

void
QMinkoEffectEditor::setupToolButtons()
{
	_qIconSave			= new QIcon(":/resources/icon-save-effect.png");
	_qIconSaveNeeded	= new QIcon(":/resources/icon-save-effect-needed.png");

	_ui->loadMkToolButton		->setIcon(QIcon(":/resources/icon-load-mk.png"));
	_ui->loadEffectToolButton	->setIcon(QIcon(":/resources/icon-load-effect.png"));

	saveNeeded(false);

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

		QObject::connect(_qAddEditorSignalMapper, SIGNAL(mapped(int)), this, SLOT(exposeQObjectsToJS(int)));
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
	saveNeeded(false);
}

void
QMinkoEffectEditor::exposeQObjectsToJS(int tabIndex)
{
	if (tabIndex < 0 || tabIndex >= NUM_TABS)
		throw std::invalid_argument("tabIndex");

	_qTabFrames[tabIndex]->addToJavaScriptWindowObject("qMinkoEffectEditor", this);
	_qTabFrames[tabIndex]->addToJavaScriptWindowObject("qTabJSObject", _qTabJSObjects[tabIndex]);
}

void
QMinkoEffectEditor::tabModified(int tabIndex, bool value)
{
	const QString& tabName	= _qTabWidget[tabIndex]->objectName();
	std::cout << "tabname[" << tabIndex << "] = " << qPrintable(tabName) << std::endl;

}

void
QMinkoEffectEditor::saveNeeded(bool value)
{
	_saveNeeded	= value;
	_ui->saveEffectToolButton->setIcon(_saveNeeded ? *_qIconSaveNeeded : *_qIconSave);
}

void
QMinkoEffectEditor::createEffect(std::string& effect) const
{
	effect.clear();

	const std::string effectName	(_ui->effectNameLineEdit->text()	.toUtf8().constData());
	const std::string srcVertex		(_qTabSources[TAB_VERTEX_SOURCE]	.toUtf8().constData());
	const std::string srcFragment	(_qTabSources[TAB_FRAGMENT_SOURCE]	.toUtf8().constData());
	const std::string srcBindings	(_qTabSources[TAB_BINDINGS]			.toUtf8().constData());

	effect = "effect name = " + effectName + "\n"
		+ "bindings = \n" + srcBindings + "\n"
		+ "vertex shader = \n" + srcVertex + "\n"
		+ "fragment shader = \n" + srcFragment + "\n";
}

void
QMinkoEffectEditor::displayEffect() const
{
	std::string effect;
	createEffect(effect);

	std::cout << "EFFECT\n" << effect << std::endl;
}

/*slot*/
void
QMinkoEffectEditor::updateEffectName()
{
	saveNeeded(true);
	displayEffect();
}

/*slot*/
void
QMinkoEffectEditor::updateSource(int tabIndex)
{
	if (tabIndex < 0 || tabIndex >= NUM_TABS)
		throw std::invalid_argument("tabIndex");

	_qTabSources[tabIndex] = _qTabFrames[TAB_VERTEX_SOURCE]->evaluateJavaScript("codeMirror.getValue()").toString();
	tabModified(tabIndex, true);

	saveNeeded(true);
	displayEffect();
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