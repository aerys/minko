#include "QMinkoEffectEditor.hpp"
#include "ui/ui_QMinkoEffectEditor.h"

QMinkoEffectEditor::QMinkoEffectEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QMinkoEffectEditor)
{
    ui->setupUi(this);
	setupSourceTabs();
}

QMinkoEffectEditor::~QMinkoEffectEditor()
{
    delete ui;
}

void
QMinkoEffectEditor::setupSourceTabs()
{
	ui->bindingsWebView->load(QUrl("qrc:///resources/minimal-codemirror.html"));
}