#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

#include "plotview.h"
#include "redpitayainterface.h"

class RedpitayaInterface;

namespace Ui {
class ViewWidget;
}

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

    void setRPif(RedpitayaInterface* ifc);

private slots:
    void on_rawplotButton_clicked();

public slots:
    void on_rawplotClsoed();

private:
    Ui::ViewWidget *ui;
    PlotView *rawplot;
    RedpitayaInterface *rpif;

    bool rawplotOpen;
};

#endif // VIEWWIDGET_H
