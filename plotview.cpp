#include "plotview.h"
#include "ui_plotview.h"

#include <QDebug>

#include <stdlib.h>
#include <stdint.h>

PlotView::PlotView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotView)
{
    ui->setupUi(this);

    x_vector =      QVector<double>(1);
    y_vector =      QVector<double>(1);

    // Set design parameter
    // create graph and assign data to it:
    ui->plot->addGraph();
    //ui->plot->graph(0)->setData(x, y);
    ui->plot->graph(0)->setPen(QPen(QColor(0x99, 0xff, 0x33, 160), 1));
    //ui->plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 5));

    // give the axes some labels:
    ui->plot->xAxis->setLabel("Sample No");
    ui->plot->yAxis->setLabel("Voltage [V]");
    ui->plot->xAxis->setLabelColor(QColor(Qt::white));
    ui->plot->xAxis->setLabelFont(QFont("Arial", 20, 1, false));
    ui->plot->yAxis->setLabelColor(QColor(Qt::white));
    ui->plot->yAxis->setLabelFont(QFont("Arial", 20, 1, false));

    // set axes ranges, so we see all data:
    ui->plot->xAxis->setRange(-1, 1);
    ui->plot->yAxis->setRange(0, 1);

    // allow interactions, drag and zoom
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);

    // design stuff
    ui->plot->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->plot->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->plot->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->plot->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->plot->xAxis->setTickLabelColor(Qt::white);
    ui->plot->yAxis->setTickLabelColor(Qt::white);
    ui->plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->plot->xAxis->grid()->setSubGridVisible(true);
    ui->plot->yAxis->grid()->setSubGridVisible(true);
    ui->plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->plot->setBackground(plotGradient);

    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    ui->plot->axisRect()->setBackground(axisRectGradient);
}

PlotView::~PlotView()
{
    delete ui;
}

void PlotView::setRPif(RedpitayaInterface* ifc)
{
    rpif = ifc;
}

void PlotView::on_quitButton_clicked()
{
    emit plotViewDestroyed();
    //this->hide();
}

void PlotView::on_dataChanged()
{
    size_t n;
    size_t numbytes = rpif->getDataArraySize();

    // allocate memory to hold the converted short values
    data_buf = (int16_t*) malloc (2*numbytes);
    if (data_buf == NULL) {fputs ("Memory error buf",stderr); exit (2);}

    // Get data
    n = rpif->getDataArray(data_buf, numbytes);

    // Copy into vectors
    x_vector.clear();
    y_vector.clear();
    x_vector.resize(n);
    y_vector.resize(n);
    for(size_t i = 0; i < n; i++)
    {
        // calculate voltage
        y_vector[i] = ((double)data_buf[i])/30678;
        x_vector[i] = i;
    }

    // Set plot data
    ui->plot->graph(0)->setData(x_vector, y_vector);
    //ui->plot->rescaleAxes();
    ui->plot->replot();

    // Doner
}

void PlotView::on_autoScaleButton_clicked()
{
    ui->plot->rescaleAxes();
    ui->plot->replot();
}

void PlotView::closeEvent (QCloseEvent *event)
{
    emit plotViewDestroyed();
}
