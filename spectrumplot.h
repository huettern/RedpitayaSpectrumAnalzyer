#ifndef SPECTRUMPLOT_H
#define SPECTRUMPLOT_H

#include <QObject>

#include "qcustomplot.h"

class QCustomPlot;

class SpectrumPlot : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumPlot(QObject *parent, QCustomPlot *qcplot);
    ~SpectrumPlot();

signals:

public slots:

private:
    QCustomPlot *m_plot;

};

#endif // SPECTRUMPLOT_H
