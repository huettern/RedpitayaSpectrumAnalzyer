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

signals:

public slots:

private:
    QCustomPlot     *m_plot;
    FFT             *m_fft;
};

#endif // SPECTRUMPLOT_H
