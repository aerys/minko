#pragma once

#include <iostream>

#include <QtCore/QSignalMapper>
#include <QtWidgets/QWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTableWidget>

namespace Ui 
{
	class QMinkoBindingsWidget;
}

class QMinkoBindingsWidget : 
	public QWidget
{
    Q_OBJECT

public:
	typedef enum
	{
		BIND_ATTRIBUTE = 0,
		BIND_UNIFORM,
		BIND_STATE,
		NUM_BIND_TYPES
	} BindingType;

private:
	Ui::QMinkoBindingsWidget			*_ui;
	QToolButton							*_qAddButtons		[NUM_BIND_TYPES];
	QToolButton							*_qRemoveButtons	[NUM_BIND_TYPES];
	QTableWidget						*_qBindingsTables	[NUM_BIND_TYPES];
	QSignalMapper						*_qAddSignalMapper;	
	QSignalMapper						*_qRemoveSignalMapper;

	std::map<std::string, std::string>	_bindings			[NUM_BIND_TYPES];

public:
    
	explicit 
	QMinkoBindingsWidget(QWidget *parent = 0);

	~QMinkoBindingsWidget();
    
	void
	addBinding(BindingType, const std::string& name, const std::string& value);

	bool
	bindingExists(BindingType, const std::string& name) const;

	const std::string&
	binding(BindingType, const std::string& name) const;

	void
	removeBinding(BindingType, const std::string& name);

private:

	void
	setupBindingsButtons();

	void
	setupBindingsTables();

signals:
	void
	bindingsChanged(const QString&);

private slots:
	void
	addBinding(int);

	void
	removeBinding(int);

	void
	addAttributeBindingAt(int row, int column);

	void
	addUniformBindingAt(int row, int column);

	void
	addStateBindingAt(int row, int column);

private:

	void
	addBindingAt(BindingType, int row, int column);
};
