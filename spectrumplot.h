#ifndef SPECTRUMPLOT_H
#define SPECTRUMPLOT_H

#include <QObject>

#include "qcustomplot.h"
#include "fft.h"

class QCustomPlot;
class FFT;

class SpectrumPlot : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumPlot(QObject *parent);
    ~SpectrumPlot();

    void setPlot (QCustomPlot *qcplot);
    void setFFT  (FFT *fft);
    void attachSharedMemory(QString key);

signals:

public slots:
    void on_dataReady();

private:
    QCustomPlot     *m_plot;
    FFT             *m_fft;
    QSharedMemory   sharedDataMemory;

    QVector<double> x_vector;
    QVector<double> y_vector;

};

#endif // SPECTRUMPLOT_H
