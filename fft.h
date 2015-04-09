#ifndef FFT_H
#define FFT_H

#include <QObject>
#include "qcustomplot.h"

class QCustomPlot;

class FFT : public QObject
{
    Q_OBJECT
public:
    explicit FFT(QObject *parent = 0, QCustomPlot *ptrplot = 0);
    ~FFT();

    void singleConversion();

private:
    QCustomPlot *plot;


signals:



public slots:


};

#endif // FFT_H
