#include "QMinkoEffectEditor.hpp"
#include "ui_QMinkoEffectEditor.h"

QMinkoEffectEditor::QMinkoEffectEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QMinkoEffectEditor)
{
    ui->setupUi(this);
}

QMinkoEffectEditor::~QMinkoEffectEditor()
{
    delete ui;
}
