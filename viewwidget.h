#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

#include "plotview.h"

namespace Ui {
class ViewWidget;
}

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

private slots:
    void on_rawplotButton_clicked();

private:
    Ui::ViewWidget *ui;
    PlotView *rawplot;

};

#endif // VIEWWIDGET_H
