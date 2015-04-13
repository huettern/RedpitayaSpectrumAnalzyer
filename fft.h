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

private:
    QCustomPlot *plot;
    RedpitayaInterface *rpif;

    // Data buffer
    int16_t* data_buf;

    int iNSamples;
    int iFFTWidth;

    //plot data
    QVector<double> x_vector;
    QVector<double> y_vector;

    void allocData();
    void freeData();
    void getRawData ();
    void plotData();


signals:



public slots:


};

#endif // FFT_H
