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
    
private:

	void
	setupBindingsButtons();

	void
	setupBindingsTables();

private slots:
	void
	addBinding(int bindType);

	void
	removeBinding(int bindType);

private:
	bool
	bindingExists(int bindType, const std::string&) const;
};
