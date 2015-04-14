#ifndef FFT_H
#define FFT_H

#include <QObject>
#include <QThread>


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

    void setThread (QThread *thr);

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

    // FFT Settings Structure
    typedef struct {

    } tstFFTSettings;

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

    QThread *thread;
    QMutex mutex;
    bool runContConv;

    void allocData();
    void freeData();
    void getRawData ();
    void plotData();
    void publishData();

signals:



public slots:
    void do_continuousConversion();


};

#endif // FFT_H
