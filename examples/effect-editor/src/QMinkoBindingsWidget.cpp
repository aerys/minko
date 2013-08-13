#include "QMinkoBindingsWidget.h"
#include "ui_QMinkoBindingsWidget.h"

QMinkoBindingsWidget::QMinkoBindingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMinkoBindingsWidget)
{
    ui->setupUi(this);
}

QMinkoBindingsWidget::~QMinkoBindingsWidget()
{
    delete ui;
}
