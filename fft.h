#ifndef FFT_H
#define FFT_H

#include <QObject>
#include <QThread>


#include "qcustomplot.h"
#include "redpitayainterface.h"

/*! Maximum number of points of the FFT. This is used to allocate memory */
#define     MAXIMUM_FFT_POINTS                      1*1024*1024


class QCustomPlot;
class RedpitayaInterface;
class QCustomPlot;


class FFT : public QObject
{
    Q_OBJECT
public:
    explicit FFT(QObject *parent = 0);
    ~FFT();

    typedef enum {
        NONE
    } tenWindows;

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
        /*! Number of ploted points */
        unsigned int visiblePoints;
        /*! Resolution per bin */
        float resolution;
        /*! Windowing method */
 //       tenWindows window;
        /*! Number of raw input sample points */
        size_t nSamples;
        /*! Number of token Samples per secons */
        unsigned int refreshRate;
        /*! Desired Number of zeroes to append to the raw samples */
        unsigned int numZeroes;
        /*! Real Number of zeroes to append to the raw samples */
        unsigned int numZeroesReal;
        /*! Time to calculate FFT */
        unsigned long elapsedTimeUs;
    } tstFFTParams;

    void setThread (QThread *thr);
    void abortThread ();

    int singleConversion();
    void setRPif(RedpitayaInterface* ifc);
    void setPlot(QCustomPlot *plt);

    void setConConversionRunning (bool state);
    void setRefreshInterval(unsigned int interval);
    void setNumZeroes(unsigned int num);

    FFT::tstFFTParams getParams();

private:
    QCustomPlot         *plot;
    RedpitayaInterface  *rpif;



    // Data buffer
    int16_t* data_buf;

    tstFFTParams FFTParams;
    int iFFTWidth;  ///< Width of the DFT, maximum number of points, incl zero
    int iPlotWidth; ///< Width of the Plot = iFFTWidth/2 + 1

    //plot data
    QVector<double> x_vector;
    QVector<double> y_vector;

    bool m_abort; //tell thread to stop
    QThread *thread;
    QMutex mutex;
    bool runContConv;

    void allocData();
    void freeData();
    void getRawData ();
    void zeroPadding();
    void convertDataToFloat();
    void plotData();
    void publishData();
    void stopContConv();

signals:
    void setStatusMsg (QString, int);
    void dataReady(QVector<double>, QVector<double>);
    void singleAcquisitionRequest();

public slots:
    void do_continuousConversion();


};

#endif // FFT_H
