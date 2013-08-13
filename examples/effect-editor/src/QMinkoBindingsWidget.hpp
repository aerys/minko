#ifndef QMINKOBINDINGSWIDGET_H
#define QMINKOBINDINGSWIDGET_H

#include <QWidget>

namespace Ui {
class QMinkoBindingsWidget;
}

class QMinkoBindingsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QMinkoBindingsWidget(QWidget *parent = 0);
    ~QMinkoBindingsWidget();
    
private:
    Ui::QMinkoBindingsWidget *ui;
};

#endif // QMINKOBINDINGSWIDGET_H
