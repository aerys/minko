#include "QMinkoBindingsWidget.hpp"
#include "ui/ui_QMinkoBindingsWidget.h"

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>

/*explicit*/
QMinkoBindingsWidget::QMinkoBindingsWidget(QWidget *parent) :
    QWidget(parent),
	_ui(new Ui::QMinkoBindingsWidget),
	_qAddSignalMapper(new QSignalMapper(this)),
	_qRemoveSignalMapper(new QSignalMapper(this))
{
    _ui->setupUi(this);

	_qAddButtons[BIND_ATTRIBUTE]					= _ui->attributeBindingsAddButton;
	_qAddButtons[BIND_UNIFORM]				= _ui->uniformBindingsAddButton;
	_qAddButtons[BIND_STATE]				= _ui->stateBindingsAddButton;

	_qRemoveButtons[BIND_ATTRIBUTE]				= _ui->attributeBindingsRemoveButton;
	_qRemoveButtons[BIND_UNIFORM]			= _ui->uniformBindingsRemoveButton;
	_qRemoveButtons[BIND_STATE]				= _ui->stateBindingsRemoveButton;

	_qBindingsTables[BIND_ATTRIBUTE]		= _ui->attributeBindingsTabWidget;
	_qBindingsTables[BIND_UNIFORM]	= _ui->uniformBindingsTabWidget;
	_qBindingsTables[BIND_STATE]		= _ui->stateBindingsTabWidget;

	setupBindingsButtons();
	setupBindingsTables();
}

QMinkoBindingsWidget::~QMinkoBindingsWidget()
{
    delete _ui;
}

void
QMinkoBindingsWidget::setupBindingsButtons()
{
	for (int i = BIND_ATTRIBUTE; i < NUM_BIND_TYPES; ++i)
	{
		_qAddButtons[i]		->setIcon(QIcon(":/resources/icon-add.png"));
		_qRemoveButtons[i]	->setIcon(QIcon(":/resources/icon-remove.png"));

		QObject::connect(_qAddButtons[i],		SIGNAL(released()), _qAddSignalMapper,		SLOT(map()));
		QObject::connect(_qRemoveButtons[i],	SIGNAL(released()), _qRemoveSignalMapper,	SLOT(map()));

		_qAddSignalMapper	->setMapping(_qAddButtons[i],		(int)i);
		_qRemoveSignalMapper->setMapping(_qRemoveButtons[i],	(int)i);
	}

	QObject::connect(_qAddSignalMapper,		SIGNAL(mapped(int)), this, SLOT(addBinding(int)));
	QObject::connect(_qRemoveSignalMapper,	SIGNAL(mapped(int)), this, SLOT(removeBinding(int)));
}

void
QMinkoBindingsWidget::setupBindingsTables()
{
	for (int i = BIND_ATTRIBUTE; i < NUM_BIND_TYPES; ++i)
	{
		_qBindingsTables[i]->setColumnCount(2);
		_qBindingsTables[i]->setHorizontalHeaderLabels(QStringList() << "Name" << "Value");
		_qBindingsTables[i]->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		_qBindingsTables[i]->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	}
}

/*slot*/
void
QMinkoBindingsWidget::addBinding(int bindType)
{
	if (bindType < 0 || bindType >= NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");

	std::string bindTypeName = "";
	switch(bindType)
	{
	case BIND_ATTRIBUTE:
		bindTypeName	= "Attribute";
		break;
	case BIND_UNIFORM:
		bindTypeName	= "Uniform";
		break;
	case BIND_STATE:
		bindTypeName	= "State";
		break;
	default:
		return;
	}

	const std::string	dialogTitle	= "New " + bindTypeName;
	const QString&		qName		= QInputDialog::getText(this, tr(dialogTitle.c_str()), tr("Property name:"));
	const std::string&	name		= qName.toUtf8().constData(); 

	if (name.empty())
		return;

	if (bindingExists(bindType, name))
	{
		const std::string	messageText	= bindTypeName + " called \'" + name + "\' already exists.";

		QMessageBox qMessage;
		qMessage.setText(tr(messageText.c_str()));
		qMessage.exec();
		return;
	}

	auto it = _bindings[bindType].insert(std::pair<std::string, std::string>(name, "\"\""));

	_qBindingsTables[bindType]->insertRow(0);
	_qBindingsTables[bindType]->setItem(0, 0, new QTableWidgetItem(tr(it.first->first.c_str())));
	_qBindingsTables[bindType]->setItem(0, 1, new QTableWidgetItem(tr(it.first->second.c_str())));
}

/*slot*/
void
QMinkoBindingsWidget::removeBinding(int bindType)
{
	if (bindType < 0 || bindType >= NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");

	std::cout << "remove bindings in " << bindType << std::endl;
}

bool
QMinkoBindingsWidget::bindingExists(int bindType, 
									const std::string& pname) const
{
	if (bindType < 0 || bindType >= NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");

	return _bindings[bindType].find(pname) != _bindings[bindType].end();
}