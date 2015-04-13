#ifndef FFT_H
#define FFT_H

#include <QObject>
#include "qcustomplot.h"
#include "redpitayainterface.h"


class QCustomPlot;
class RedpitayaInterface;
class QCustomPlot;


class FFT : public QObject
{
    Q_OBJECT
public:
    explicit FFT(QObject *parent = 0);
    ~FFT();

    void singleConversion();
    void setRPif(RedpitayaInterface* ifc);
    void setPlot(QCustomPlot *plt);

    // Data structure
    typedef struct {
        QVector<double> mag;
        QVector<double> freq;
        int width;
        double binsize;
    } tstdata;
    tstdata data;

private:
    QCustomPlot *plot;
    RedpitayaInterface *rpif;

    // Data buffer
    int16_t* data_buf;

    int iNSamples;
    int iFFTWidth;  ///< Width of the DFT, maximum number of points
    int iPlotWidth; ///< Width of the Plot = iFFTWidth/2 + 1

    //plot data
    QVector<double> x_vector;
    QVector<double> y_vector;

    QMutex mutex;

    void allocData();
    void freeData();
    void getRawData ();
    void plotData();
    void publishData();

signals:



public slots:


};

#endif // FFT_H
