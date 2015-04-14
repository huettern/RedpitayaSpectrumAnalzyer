#include "viewwidget.h"
#include "ui_viewwidget.h"

#include <QDebug>

ViewWidget::ViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewWidget)
{
    ui->setupUi(this);

    rawplotOpen = false;
    //rawplot = new PlotView();
}

ViewWidget::~ViewWidget()
{
    delete ui;
}

void ViewWidget::on_rawplotButton_clicked()
{
    if(rawplotOpen == true)
    {
        rawplot->raise();
        return;
    }
    rawplotOpen = true;

    rawplot = new PlotView(this);
    rawplot->setRPif(rpif);
    connect(rawplot, SIGNAL(plotViewDestroyed()),
            this,    SLOT(on_rawplotClsoed()));
    connect(rpif,    SIGNAL(dataReady()),
            rawplot, SLOT(on_dataChanged()));
    rawplot->show();
}

void ViewWidget::setRPif(RedpitayaInterface* ifc)
{
    rpif = ifc;
}

void ViewWidget::setSpectrumPlot(QCustomPlot *plt)
{
    spectrumPlot = plt;
}

void ViewWidget::on_rawplotClsoed()
{
    rawplotOpen = false;
    delete rawplot;
}

void ViewWidget::on_autoscalePlot_clicked()
{
    spectrumPlot->rescaleAxes();
    spectrumPlot->replot();
}
