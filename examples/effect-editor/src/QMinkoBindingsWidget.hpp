#pragma once

#include <iostream>
#include <unordered_map>

#include <QtCore/QSignalMapper>
#include <QtWidgets/QWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTableWidget>

#include "minko/render/BindingType.hpp"

namespace Ui 
{
	class QMinkoBindingsWidget;
}

class QMinkoBindingsWidget : 
	public QWidget
{
    Q_OBJECT

private:
	typedef	std::unordered_map<std::string, std::string> BindingMap;
private:
	Ui::QMinkoBindingsWidget	*_ui;
	QToolButton					*_qAddButtons		[minko::render::NUM_BIND_TYPES];
	QToolButton					*_qRemoveButtons	[minko::render::NUM_BIND_TYPES];
	QTableWidget				*_qBindingsTables	[minko::render::NUM_BIND_TYPES];
	QSignalMapper				*_qAddSignalMapper;	
	QSignalMapper				*_qRemoveSignalMapper;
	BindingMap					_bindings			[minko::render::NUM_BIND_TYPES];

public:
	explicit 
	QMinkoBindingsWidget(QWidget *parent = 0);

	~QMinkoBindingsWidget();
    
	void
	addBinding(minko::render::BindingType, const std::string& name, const std::string& value);

	bool
	bindingExists(minko::render::BindingType, const std::string& name) const;

	const std::string&
	binding(minko::render::BindingType, const std::string& name) const;

	const BindingMap&
	bindings(minko::render::BindingType) const;

	void
	bindings(minko::render::BindingType, const BindingMap&);

	void
	removeBinding(minko::render::BindingType, const std::string& name);

private:

	void
	setupBindingsButtons();

	void
	setupBindingsTables();

signals:
	void
	bindingsChanged(minko::render::BindingType);

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

	void
	addMacroBindingAt(int row, int column);

private:
	void
	addBindingAt(minko::render::BindingType, int row, int column);

	void
	appendBindingTableRow(minko::render::BindingType, const std::string& name, const std::string& value);
};
