#include "QMinkoBindingsWidget.hpp"
#include "ui/ui_QMinkoBindingsWidget.h"

#include <set>
#include <sstream>

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

	_qAddButtons[BIND_ATTRIBUTE]		= _ui->attributeBindingsAddButton;
	_qAddButtons[BIND_UNIFORM]			= _ui->uniformBindingsAddButton;
	_qAddButtons[BIND_STATE]			= _ui->stateBindingsAddButton;

	_qRemoveButtons[BIND_ATTRIBUTE]		= _ui->attributeBindingsRemoveButton;
	_qRemoveButtons[BIND_UNIFORM]		= _ui->uniformBindingsRemoveButton;
	_qRemoveButtons[BIND_STATE]			= _ui->stateBindingsRemoveButton;

	_qBindingsTables[BIND_ATTRIBUTE]	= _ui->attributeBindingsTabWidget;
	_qBindingsTables[BIND_UNIFORM]		= _ui->uniformBindingsTabWidget;
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

	QObject::connect(_qBindingsTables[BIND_ATTRIBUTE],	SIGNAL(cellChanged(int, int)), this, SLOT(addAttributeBindingAt(int, int)));
	QObject::connect(_qBindingsTables[BIND_UNIFORM],	SIGNAL(cellChanged(int, int)), this, SLOT(addUniformBindingAt(int, int)));
	QObject::connect(_qBindingsTables[BIND_STATE],		SIGNAL(cellChanged(int, int)), this, SLOT(addStateBindingAt(int, int)));
}

/*slot*/
void
QMinkoBindingsWidget::addBinding(int bindTypeId)
{
#ifdef DEBUG
	if (bindTypeId < 0 || bindTypeId >= NUM_BIND_TYPES)
		throw std::invalid_argument("bindTypeId");
#endif // DEBUG

	const BindingType	bindType		= (BindingType)bindTypeId;
	std::string			bindTypeName	= "";
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

	QTableWidgetItem* nameItem	= new QTableWidgetItem(tr(name.c_str()));
	QTableWidgetItem* valueItem	= new QTableWidgetItem(tr("\" \""));
	nameItem->setFlags(Qt::NoItemFlags);

	_qBindingsTables[bindType]->insertRow(0);
	_qBindingsTables[bindType]->setItem(0, 0, nameItem);
	_qBindingsTables[bindType]->setItem(0, 1, valueItem);
}

/*slot*/
void
QMinkoBindingsWidget::removeBinding(int bindTypeId)
{
#ifdef DEBUG
	if (bindTypeId < 0 || bindTypeId >= NUM_BIND_TYPES)
		throw std::invalid_argument("bindTypeId");
#endif // DEBUG
	
	// keep the set of keys to erase
	std::set<std::string>		names;

	const BindingType			bindType	= (BindingType)bindTypeId;
	QList<QTableWidgetItem*>	qSelection	= _qBindingsTables[bindType]->selectedItems();
	while (!qSelection.isEmpty())
	{
		const QTableWidgetItem* qItem		= qSelection.at(0);
		const int				row			= qItem->row();

		const QTableWidgetItem* qReferenceItem	= _qBindingsTables[bindType]->itemAt(row, 0);
		names.insert(std::string(qReferenceItem->text().toUtf8().constData()));

		_qBindingsTables[bindType]->removeRow(row);

		qSelection = _qBindingsTables[bindType]->selectedItems();
	}

	for (auto name : names)
		removeBinding(bindType, name);
}

/*slot*/
void
QMinkoBindingsWidget::addAttributeBindingAt(int row, int column)
{
	addBindingAt(BIND_ATTRIBUTE, row, column);
}

/*slot*/
void
QMinkoBindingsWidget::addUniformBindingAt(int row, int column)
{
	addBindingAt(BIND_UNIFORM, row, column);
}

/*slot*/
void
QMinkoBindingsWidget::addStateBindingAt(int row, int column)
{
	addBindingAt(BIND_STATE, row, column);
}

void
QMinkoBindingsWidget::addBindingAt(QMinkoBindingsWidget::BindingType bindType, 
									int row, 
									int column)
{
#ifdef DEBUG
	if (bindType == NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");
	if (row < 0 || row >= _qBindingsTables[bindType]->rowCount())
		throw std::invalid_argument("row");
	if (column < 0 || column >= _qBindingsTables[bindType]->columnCount())
		throw std::invalid_argument("column");
#endif // DEBUG

	const QTableWidgetItem* nameItem	= _qBindingsTables[bindType]->item(row, 0);
	const QTableWidgetItem* valueItem	= _qBindingsTables[bindType]->item(row, 1);

	if (nameItem == nullptr || valueItem == nullptr)
		return; // missing information. most likely, the table row is being created.

	addBinding(
		bindType,
		nameItem->text().toUtf8().constData(),
		valueItem->text().toUtf8().constData()
	);
}

bool
QMinkoBindingsWidget::bindingExists(QMinkoBindingsWidget::BindingType bindType, 
									const std::string& name) const
{
#ifdef DEBUG
	if (bindType == NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");
#endif // DEBUG

	return _bindings[bindType].find(name) != _bindings[bindType].end();
}

void
QMinkoBindingsWidget::removeBinding(QMinkoBindingsWidget::BindingType bindType, 
									const std::string& name)
{
#ifdef DEBUG
	if (bindType == NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");
#endif // DEBUG

	auto it = _bindings[bindType].find(name);
	if (it != _bindings[bindType].end())
	{
#ifdef DEBUG
		std::cout << "will erase \'" << it->first << "\'" << std::endl;
#endif // DEBUG

		_bindings[bindType].erase(it);
	}
}

void
QMinkoBindingsWidget::addBinding(QMinkoBindingsWidget::BindingType bindType, 
								const std::string& name, 
								const std::string& value)
{
#ifdef DEBUG
	if (bindType == NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");
	if (name.empty())
		throw std::invalid_argument("name");
#endif // DEBUG

	_bindings[bindType][name] = value;

#ifdef DEBUG
	std::cout << "added bindings[" << bindType << "][" << name << "] = " << _bindings[bindType][name] << std::endl;
#endif // DEBUG
}

const std::string&
QMinkoBindingsWidget::binding(QMinkoBindingsWidget::BindingType bindType, 
							const std::string& name) const
{
#ifdef DEBUG
	if (bindType == NUM_BIND_TYPES)
		throw std::invalid_argument("bindType");
#endif // DEBUG

	auto it = _bindings[bindType].find(name);

	if (it == _bindings[bindType].end())
	{
		std::stringstream stream;
		stream << "\'" << name << "\' is not a valid binding";
		throw std::logic_error(stream.str());
	}

	return it->second;
}
