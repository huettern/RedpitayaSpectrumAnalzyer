#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

#include "plotview.h"
#include "redpitayainterface.h"
#include "qcustomplot.h"

class RedpitayaInterface;
class QCustomPlot;

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
    void setSpectrumPlot(QCustomPlot *plt);

private slots:
    void on_rawplotButton_clicked();

    void on_autoscalePlot_clicked();

public slots:
    void on_rawplotClsoed();

private:
    Ui::ViewWidget *ui;
    PlotView *rawplot;
    RedpitayaInterface *rpif;
    QCustomPlot *spectrumPlot;

    bool rawplotOpen;
};

#endif // VIEWWIDGET_H
