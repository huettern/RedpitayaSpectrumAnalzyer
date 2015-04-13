#include "spectrumplot.h"

SpectrumPlot::SpectrumPlot(QObject *parent, QCustomPlot *qcplot) : QObject(parent)
{
    // save plot handle
    m_plot = qcplot;

    // Set design parameter
    // create graph and assign data to it:
    m_plot->addGraph();
    //m_plot->graph(0)->setData(x, y);
    m_plot->graph(0)->setPen(QPen(QColor(0x99, 0xff, 0x33, 160), 1));
    //m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 5));

    // give the axes some labels:
    m_plot->xAxis->setLabel("x");
    m_plot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    m_plot->xAxis->setRange(-1, 1);
    m_plot->yAxis->setRange(0, 1);

    // allow interactions, drag and zoom
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);

    // design stuff
    m_plot->xAxis->setBasePen(QPen(Qt::white, 1));
    m_plot->yAxis->setBasePen(QPen(Qt::white, 1));
    m_plot->xAxis->setTickPen(QPen(Qt::white, 1));
    m_plot->yAxis->setTickPen(QPen(Qt::white, 1));
    m_plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    m_plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    m_plot->xAxis->setTickLabelColor(Qt::white);
    m_plot->yAxis->setTickLabelColor(Qt::white);
    m_plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    m_plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    m_plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    m_plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    m_plot->xAxis->grid()->setSubGridVisible(true);
    m_plot->yAxis->grid()->setSubGridVisible(true);
    m_plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    m_plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    m_plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    m_plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    m_plot->setBackground(plotGradient);

    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    m_plot->axisRect()->setBackground(axisRectGradient);

    m_plot->replot();
}

SpectrumPlot::~SpectrumPlot()
{

}

