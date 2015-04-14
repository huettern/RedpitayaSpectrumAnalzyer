#include "fft.h"

#include <math.h>
#include <qdebug.h>

//  **************************************************************************
//  DEFINES
//  **************************************************************************
#define     MIA_FFT_IDEAL_FILTER					0
#define     MIA_FFT_BUTTERWORTH_FILTER				1
#define     MIA_FFT_GAUSS_FILTER					2


//  **************************************************************************
//  TEMPLATE FUNCTIONS
//  **************************************************************************
template<typename T1_> void fMIA_FFT1D(int iWidth,  T1_ *pa,
                                       int  iSign, int *iWsp, T1_ *Wsp);
template<typename T1_> void fMIA_FFT1D_AmplSpec(int iWidth,
                                                T1_ *pIn, T1_ *pOut);


float             *flData;      // in Daten
int               *ip;          // integer Workspace
float             *w;           // float Workspace
float             *flAmpSpec;   // spectrum



/****************************************************************************
 * PUBLIC SECTION
 -----------------------------------------------------------------------*//**
 * @publicsection
 ****************************************************************************/
FFT::FFT(QObject *parent) : QObject(parent)
{
    runContConv = false;
}

FFT::~FFT()
{
}

void FFT::setThread (QThread *thr)
{
    connect(thr, SIGNAL(started()), this, SLOT(do_continuousConversion()));
}

/**
 * @brief singleConversion
 *
 * Calculates the FFT of a single set of Data
 */
void FFT::singleConversion()
{
    QElapsedTimer tim;
    tim.start();
    // Get Data
    getRawData();

    iFFTWidth = iNSamples;
    iPlotWidth = (iFFTWidth/2) + 1;

    // Allocate memory for FFT
    allocData();

    // Calculate Data
    ip[0] = 0;
    fMIA_FFT1D(iFFTWidth, flData, 1 , ip, w); // Fourier Transforamtion
    fMIA_FFT1D_AmplSpec(iFFTWidth, flData, flAmpSpec); // Spektrum

    // Display Data
    plotData();

    // Publish Data
    publishData();

    // clean up
    freeData();
    qDebug() << "FFT time" << tim.nsecsElapsed();
}


void FFT::setRPif(RedpitayaInterface* ifc)
{
    rpif = ifc;
}

void FFT::setPlot(QCustomPlot *plt)
{
    plot = plt;
}


/****************************************************************************
 * PUBLIC SLOTS SECTION
 -----------------------------------------------------------------------*//**
 * @publicslotssection
 ****************************************************************************/
void FFT::do_continuousConversion()
{

//    while(true)
//    {
//        thread->usleep(1000000);

//        mutex.lock();
//        if(runContConv == true)
//        {
//            mutex.unlock();
//            thread->usleep(1000000);


//        }
//        mutex.unlock();
//    }
}

/****************************************************************************
 * PRIATE SECTION
 -----------------------------------------------------------------------*//**
 * @privatesection
 ****************************************************************************/

void FFT::allocData ()
{
    qDebug() << "Allocating with iFFTWidth = " << iFFTWidth;
    ip = (int*) malloc (sizeof(int)*2*iFFTWidth);
    if (ip == NULL) {fputs ("Memory error buf",stderr); exit (2);}
    w = (float*) malloc (sizeof(float)*2*iFFTWidth);
    if (w == NULL) {fputs ("Memory error buf",stderr); exit (2);}
    flAmpSpec = (float*) malloc (sizeof(float)*2*iFFTWidth);
    if (flAmpSpec == NULL) {fputs ("Memory error buf",stderr); exit (2);}
}

void FFT::freeData ()
{
    free(data_buf);
    free(flData);
    free(ip);
    free(w);
    free(flAmpSpec);
    x_vector.clear();
    y_vector.clear();
}

void FFT::getRawData ()
{
    qDebug() << "on_dataChanged";
    iNSamples = rpif->getDataArraySize();

    // allocate memory to hold the converted short values
    data_buf = (int16_t*) malloc (sizeof(int16_t)*iNSamples);
    if (data_buf == NULL) {fputs ("Memory error buf",stderr); exit (2);}
    flData = (float*) malloc (sizeof(float)*iNSamples);
    if (flData == NULL) {fputs ("Memory error buf",stderr); exit (2);}

    // Get data
    rpif->getDataArray(data_buf, iNSamples);

    //convert to float
    for(int i = 0; i < iNSamples; i++)
    {
        flData[i] = ((float)data_buf[i])/30678;
    }
}

void FFT::plotData()
{
    double rate = rpif->getSamplerate();
    // convert to QVector
    x_vector.clear();
    y_vector.clear();
    x_vector.resize(iPlotWidth);
    y_vector.resize(iPlotWidth);

    for(int i = 0; i < iPlotWidth; i++)
    {
        y_vector[i] = (double)flAmpSpec[i] / (iFFTWidth/2);
        x_vector[i] = i*(rate/iFFTWidth);
    }

    // Set plot data
    plot->graph(0)->setData(x_vector, y_vector);
    plot->rescaleAxes();
    plot->replot();
}

void FFT::publishData()
{
    mutex.lock();

    data.mag.resize(iFFTWidth);
    data.freq.resize(iFFTWidth);

    memcpy(data.mag.data(), y_vector.data(), iPlotWidth);
    memcpy(data.freq.data(), x_vector.data(), iPlotWidth);

    data.width = iFFTWidth;
    data.binsize = rpif->getSamplerate() / iFFTWidth;

    mutex.unlock();
}

/****************************************************************************
 * MIAFFT SECTION
 -----------------------------------------------------------------------*//**
 * @MIAFFT
 ****************************************************************************/

/****************************************************************************
 * HELPER FUNCTIONS
 ****************************************************************************/
//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void makewt(int nw, int *iWsp, T1_ *Wsp)
{
    int		nwh, j;
    T1_		delta, x, y;

    iWsp[0] = nw;
    iWsp[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = (T1_)atan(1.0) / nwh;
        Wsp[0] = 1;
        Wsp[1] = 0;
        Wsp[nwh] = cos(delta * nwh);
        Wsp[nwh + 1] = Wsp[nwh];
        for (j = 2; j < nwh ; j += 2) {
            x = cos(delta * j);
            y = sin(delta * j);
            Wsp[j] = x;
            Wsp[j + 1] = y;
            Wsp[nw - j] = y;
            Wsp[nw - j + 1] = x;
        }
        bitrv2(nw, iWsp + 2, w);
    }
}


//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void makect(int nc, int *iWsp, T1_ *Wsp)
{
    int		nch, j;
    T1_		delta;

    iWsp[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        delta = (T1_)(atan(1.0) / nch);
        Wsp[0] = 0.5f;
        Wsp[nch] = (T1_)(0.5f * cos(delta * nch));
        for (j = 1; j < nch ; j++) {
            Wsp[j] = (T1_)(0.5f * cos(delta * j));
            Wsp[nc - j] = (T1_)(0.5f * sin(delta * j));
        }
    }
}


//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void bitrv2(int n, int *iWsp, T1_ *pa)
{
    int		j, j1, k, k1, l, m, m2;
    T1_		xr, xi;

    iWsp[0] = 0;
    l = n;
    m = 1;
    while ((m << 2) < l) {
        l >>= 1;
         for (j = 0; j < m ; j++) {
            iWsp[m + j] = iWsp[j] + l;
        }
        m <<= 1;
    }
    if ((m << 2) > l) {
        for (k = 1; k < m ; k++) {
            for (j = 0; j < k ; j++) {
                j1 = (j << 1) + iWsp[k];
                k1 = (k << 1) + iWsp[j];
                xr = pa[j1];
                xi = pa[j1 + 1];
                pa[j1] = pa[k1];
                pa[j1 + 1] = pa[k1 + 1];
                pa[k1] = xr;
                pa[k1 + 1] = xi;
            }
        }
    } else {
        m2 = m << 1;
        for (k = 1; k < m ; k++) {
            for (j = 0; j < k ; j++) {
                j1 = (j << 1) + iWsp[k];
                k1 = (k << 1) + iWsp[j];
                xr = pa[j1];
                xi = pa[j1 + 1];
                pa[j1] = pa[k1];
                pa[j1 + 1] = pa[k1 + 1];
                pa[k1] = xr;
                pa[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = pa[j1];
                xi = pa[j1 + 1];
                pa[j1] = pa[k1];
                pa[j1 + 1] = pa[k1 + 1];
                pa[k1] = xr;
                pa[k1 + 1] = xi;
            }
        }
    }
}



//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void bitrv2col(int iHeight, int iWidth, int *iWsp, T1_ *pa)
{
    int		i, j, j1, k, k1, l, m, m2;
    T1_		xr, xi;
    T1_		*pai;

    iWsp[0] = 0;
    l = iWidth;
    m = 1;
    while ((m << 2) < l) {
        l >>= 1;
        for (j = 0; j < m ; j++) {
            iWsp[m + j] = iWsp[j] + l;
        }
        m <<= 1;
    }
    if ((m << 2) > l) {
        for (i = 0; i < iHeight ; i++) {
            pai = &pa[i*iWidth];
            for (k = 1; k < m ; k++) {
                for (j = 0; j < k ; j++) {
                    j1 = (j << 1) + iWsp[k];
                    k1 = (k << 1) + iWsp[j];
                    xr = pai[j1];
                    xi = pai[j1 + 1];
                    pai[j1] = pai[k1];
                    pai[j1 + 1] = pai[k1 + 1];
                    pai[k1] = xr;
                    pai[k1 + 1] = xi;
                }
            }
        }
    } else {
        m2 = m << 1;
          for (i = 0; i < iHeight ; i++) {
            pai = &pa[i*iWidth];
            for (k = 1; k < m ; k++) {
                for (j = 0; j < k ; j++) {
                    j1 = (j << 1) + iWsp[k];
                    k1 = (k << 1) + iWsp[j];
                    xr = pai[j1];
                    xi = pai[j1 + 1];
                    pai[j1] = pai[k1];
                    pai[j1 + 1] = pai[k1 + 1];
                    pai[k1] = xr;
                    pai[k1 + 1] = xi;
                    j1 += m2;
                    k1 += m2;
                    xr = pai[j1];
                    xi = pai[j1 + 1];
                    pai[j1] = pai[k1];
                    pai[j1 + 1] = pai[k1 + 1];
                    pai[k1] = xr;
                    pai[k1 + 1] = xi;
                }
            }
        }
    }
}




//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void bitrv2row(int iHeight, int iWidth, int *iWsp, T1_ *pa)
{
    T1_		*paj,*pak;
    T1_		xr, xi;
    int		i, j, k,  l, m;


    iWsp[0] = 0;
    l = iHeight;
    m = 1;
    while ((m << 1) < l) {
        l >>= 1;
        for (j = 0; j < m ; j++) {
            iWsp[m + j] = iWsp[j] + l;
        }
        m <<= 1;
    }
    if ((m << 1) > l) {
        for (k = 1; k < m ; k++) {
            for (j = 0; j < k ; j++) {
                paj = &pa[(j + iWsp[k])*iWidth];
                pak = &pa[(k + iWsp[j])*iWidth];
                for (i = 0; i < iWidth ; i += 2) {
                    xr = paj[i];
                    xi = paj[i + 1];
                    paj[i] = pak[i];
                    paj[i + 1] = pak[i + 1];
                    pak[i] = xr;
                    pak[i + 1] = xi;
                }
            }
        }
    } else {
        for (k = 1; k < m ; k++) {
            for (j = 0; j < k ; j++) {
                paj = &pa[(j + iWsp[k])*iWidth];
                pak = &pa[(k + iWsp[j])*iWidth];
                for (i = 0; i < iWidth ; i += 2) {
                    xr = paj[i];
                    xi = paj[i + 1];
                    paj[i] = pak[i];
                    paj[i + 1] = pak[i + 1];
                    pak[i] = xr;
                    pak[i + 1] = xi;
                }
                paj = &pa[(j + iWsp[k] + m)*iWidth];
                pak = &pa[(k + iWsp[j] + m)*iWidth];
                for (i = 0; i < iWidth ; i += 2) {
                    xr = paj[i];
                    xi = paj[i + 1];
                    paj[i] = pak[i];
                    paj[i + 1] = pak[i + 1];
                    pak[i] = xr;
                    pak[i + 1] = xi;
                }
            }
        }
    }
}





//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void cftbcol(int iHeight, int iWidth, T1_ *pa, T1_ *Wsp)
{
    int		i, j, j1, j2, j3, k, k1, ks, l, m;
    T1_		wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    T1_		x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    T1_ *pai;

    for (i = 0; i < iHeight ; i++) {
        l = 2;
        pai = &pa[i*iWidth];
        while ((l << 1) < iWidth) {
            m = l << 2;
             for (j = 0; j < l ; j += 2) {
                j1 = j + l;
                j2 = j1 + l;
                j3 = j2 + l;
                x0r = pai[j] + pai[j1];
                x0i = pai[j + 1] + pai[j1 + 1];
                x1r = pai[j] - pai[j1];
                x1i = pai[j + 1] - pai[j1 + 1];
                x2r = pai[j2] + pai[j3];
                x2i = pai[j2 + 1] + pai[j3 + 1];
                x3r = pai[j2] - pai[j3];
                x3i = pai[j2 + 1] - pai[j3 + 1];
                pai[j] = x0r + x2r;
                pai[j + 1] = x0i + x2i;
                pai[j2] = x0r - x2r;
                pai[j2 + 1] = x0i - x2i;
                pai[j1] = x1r - x3i;
                pai[j1 + 1] = x1i + x3r;
                pai[j3] = x1r + x3i;
                pai[j3 + 1] = x1i - x3r;
            }
            if (m < iWidth) {
                wk1r = Wsp[2];
                for (j = m; j < l + m ; j += 2) {
                    j1 = j + l;
                    j2 = j1 + l;
                    j3 = j2 + l;
                    x0r = pai[j] + pai[j1];
                    x0i = pai[j + 1] + pai[j1 + 1];
                    x1r = pai[j] - pai[j1];
                    x1i = pai[j + 1] - pai[j1 + 1];
                    x2r = pai[j2] + pai[j3];
                    x2i = pai[j2 + 1] + pai[j3 + 1];
                    x3r = pai[j2] - pai[j3];
                    x3i = pai[j2 + 1] - pai[j3 + 1];
                    pai[j] = x0r + x2r;
                    pai[j + 1] = x0i + x2i;
                    pai[j2] = x2i - x0i;
                    pai[j2 + 1] = x0r - x2r;
                    x0r = x1r - x3i;
                    x0i = x1i + x3r;
                    pai[j1] = wk1r * (x0r - x0i);
                    pai[j1 + 1] = wk1r * (x0r + x0i);
                    x0r = x3i + x1r;
                    x0i = x3r - x1i;
                    pai[j3] = wk1r * (x0i - x0r);
                    pai[j3 + 1] = wk1r * (x0i + x0r);
                }
                k1 = 1;
                ks = -1;
                for (k = (m << 1); k < iWidth ; k += m) {
                    k1++;
                    ks = -ks;
                    wk1r = Wsp[k1 << 1];
                    wk1i = Wsp[(k1 << 1) + 1];
                    wk2r = ks * Wsp[k1];
                    wk2i = Wsp[k1 + ks];
                    wk3r = wk1r - 2 * wk2i * wk1i;
                    wk3i = 2 * wk2i * wk1r - wk1i;
                    for (j = k; j < l + k ; j += 2) {
                        j1 = j + l;
                        j2 = j1 + l;
                        j3 = j2 + l;
                        x0r = pai[j] + pai[j1];
                        x0i = pai[j + 1] + pai[j1 + 1];
                        x1r = pai[j] - pai[j1];
                        x1i = pai[j + 1] - pai[j1 + 1];
                        x2r = pai[j2] + pai[j3];
                        x2i = pai[j2 + 1] + pai[j3 + 1];
                        x3r = pai[j2] - pai[j3];
                        x3i = pai[j2 + 1] - pai[j3 + 1];
                        pai[j] = x0r + x2r;
                        pai[j + 1] = x0i + x2i;
                        x0r -= x2r;
                        x0i -= x2i;
                        pai[j2] = wk2r * x0r - wk2i * x0i;
                        pai[j2 + 1] = wk2r * x0i + wk2i * x0r;
                        x0r = x1r - x3i;
                        x0i = x1i + x3r;
                        pai[j1] = wk1r * x0r - wk1i * x0i;
                        pai[j1 + 1] = wk1r * x0i + wk1i * x0r;
                        x0r = x1r + x3i;
                        x0i = x1i - x3r;
                        pai[j3] = wk3r * x0r - wk3i * x0i;
                        pai[j3 + 1] = wk3r * x0i + wk3i * x0r;
                    }
                }
            }
            l = m;
        }
        if (l < iWidth) {
            for (j = 0; j < l ; j += 2) {
                j1 = j + l;
                x0r = pai[j] - pai[j1];
                x0i = pai[j + 1] - pai[j1 + 1];
                pai[j] += pai[j1];
                pai[j + 1] += pai[j1 + 1];
                pai[j1] = x0r;
                pai[j1 + 1] = x0i;
            }
        }
    }
}



//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void cftbrow(int iHeight, int iWidth, T1_ *pa, T1_ *Wsp)
{
    int		i, j, k, k1, ks, l, ll, m;
    T1_		wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    T1_		x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    T1_		*paj0, *paj1, *paj2, *paj3;

    l = 1;
    while ((l << 1) < iHeight) {
        m = l << 2;
        ll = l*iWidth;
        for (j = 0; j < l; j++) {
            paj0 = &pa[j*iWidth];
            paj1 = &paj0[ll];
            paj2 = &paj1[ll];
            paj3 = &paj2[ll];
            for (i = 0; i < iWidth ; i += 2) {
                x0r = paj0[i] + paj1[i];
                x0i = paj0[i + 1] + paj1[i + 1];
                x1r = paj0[i] - paj1[i];
                x1i = paj0[i + 1] - paj1[i + 1];
                x2r = paj2[i] + paj3[i];
                x2i = paj2[i + 1] + paj3[i + 1];
                x3r = paj2[i] - paj3[i];
                x3i = paj2[i + 1] - paj3[i + 1];
                paj0[i] = x0r + x2r;
                paj0[i + 1] = x0i + x2i;
                paj2[i] = x0r - x2r;
                paj2[i + 1] = x0i - x2i;
                paj1[i] = x1r - x3i;
                paj1[i + 1] = x1i + x3r;
                paj3[i] = x1r + x3i;
                paj3[i + 1] = x1i - x3r;
            }
        }
        if (m < iHeight) {
            wk1r = Wsp[2];
            for (j = m; j < l + m ; j++) {
                paj0 = &pa[j*iWidth];
                paj1 = &paj0[ll];
                paj2 = &paj1[ll];
                paj3 = &paj2[ll];
                for (i = 0; i < iWidth ; i += 2) {
                    x0r = paj0[i] + paj1[i];
                    x0i = paj0[i + 1] + paj1[i + 1];
                    x1r = paj0[i] - paj1[i];
                    x1i = paj0[i + 1] - paj1[i + 1];
                    x2r = paj2[i] + paj3[i];
                    x2i = paj2[i + 1] + paj3[i + 1];
                    x3r = paj2[i] - paj3[i];
                    x3i = paj2[i + 1] - paj3[i + 1];
                    paj0[i] = x0r + x2r;
                    paj0[i + 1] = x0i + x2i;
                    paj2[i] = x2i - x0i;
                    paj2[i + 1] = x0r - x2r;
                    x0r = x1r - x3i;
                    x0i = x1i + x3r;
                    paj1[i] = wk1r * (x0r - x0i);
                    paj1[i + 1] = wk1r * (x0r + x0i);
                    x0r = x3i + x1r;
                    x0i = x3r - x1i;
                    paj3[i] = wk1r * (x0i - x0r);
                    paj3[i + 1] = wk1r * (x0i + x0r);
                }
            }
            k1 = 1;
            ks = -1;
            for (k = (m << 1); k < iHeight; k += m) {
                k1++;
                ks = -ks;
                wk1r = Wsp[k1 << 1];
                wk1i = Wsp[(k1 << 1) + 1];
                wk2r = ks * Wsp[k1];
                wk2i = Wsp[k1 + ks];
                wk3r = wk1r - 2 * wk2i * wk1i;
                wk3i = 2 * wk2i * wk1r - wk1i;
                for (j = k; j < l + k ; j++) {
                    paj0 = &pa[j*iWidth];
                    paj1 = &paj0[ll];
                    paj2 = &paj1[ll];
                    paj3 = &paj2[ll];
                    for (i = 0; i < iWidth ; i += 2) {
                        x0r = paj0[i] + paj1[i];
                        x0i = paj0[i + 1] + paj1[i + 1];
                        x1r = paj0[i] - paj1[i];
                        x1i = paj0[i + 1] - paj1[i + 1];
                        x2r = paj2[i] + paj3[i];
                        x2i = paj2[i + 1] + paj3[i + 1];
                        x3r = paj2[i] - paj3[i];
                        x3i = paj2[i + 1] - paj3[i + 1];
                        paj0[i] = x0r + x2r;
                        paj0[i + 1] = x0i + x2i;
                        x0r -= x2r;
                        x0i -= x2i;
                        paj2[i] = wk2r * x0r - wk2i * x0i;
                        paj2[i + 1] = wk2r * x0i + wk2i * x0r;
                        x0r = x1r - x3i;
                        x0i = x1i + x3r;
                        paj1[i] = wk1r * x0r - wk1i * x0i;
                        paj1[i + 1] = wk1r * x0i + wk1i * x0r;
                        x0r = x1r + x3i;
                        x0i = x1i - x3r;
                        paj3[i] = wk3r * x0r - wk3i * x0i;
                        paj3[i + 1] = wk3r * x0i + wk3i * x0r;
                    }
                }
            }
        }
        l = m;
    }
    if (l < iHeight) {
         for (j = 0; j < l ; j++) {
            paj0 = &pa[j*iWidth];
            paj1 = &pa[(j + l)*iWidth];
            for (i = 0; i < iWidth ; i += 2) {
                x0r = paj0[i] - paj1[i];
                x0i = paj0[i + 1] - paj1[i + 1];
                paj0[i] += paj1[i];
                paj0[i + 1] += paj1[i + 1];
                paj1[i] = x0r;
                paj1[i + 1] = x0i;
            }
        }
    }
}


//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void cftfcol(int iHeight, int iWidth, T1_ *pa, T1_ *Wsp)
{
    int		i, j, j1, j2, j3, k, k1, ks, l, m;
    T1_		wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    T1_		x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    T1_		*pai;

    for (i = 0; i < iHeight ; i++) {
        l = 2;
        pai = &pa[i*iWidth];
        while ((l << 1) < iWidth) {
            m = l << 2;
            for (j = 0; j < l ; j += 2) {
                j1 = j + l;
                j2 = j1 + l;
                j3 = j2 + l;
                x0r = pai[j] + pai[j1];
                x0i = pai[j + 1] + pai[j1 + 1];
                x1r = pai[j] - pai[j1];
                x1i = pai[j + 1] - pai[j1 + 1];
                x2r = pai[j2] + pai[j3];
                x2i = pai[j2 + 1] + pai[j3 + 1];
                x3r = pai[j2] - pai[j3];
                x3i = pai[j2 + 1] - pai[j3 + 1];
                pai[j] = x0r + x2r;
                pai[j + 1] = x0i + x2i;
                pai[j2] = x0r - x2r;
                pai[j2 + 1] = x0i - x2i;
                pai[j1] = x1r + x3i;
                pai[j1 + 1] = x1i - x3r;
                pai[j3] = x1r - x3i;
                pai[j3 + 1] = x1i + x3r;
            }
            if (m < iWidth) {
                wk1r = Wsp[2];
                for (j = m; j < l + m ; j += 2) {
                    j1 = j + l;
                    j2 = j1 + l;
                    j3 = j2 + l;
                    x0r = pai[j] + pai[j1];
                    x0i = pai[j + 1] + pai[j1 + 1];
                    x1r = pai[j] - pai[j1];
                    x1i = pai[j + 1] - pai[j1 + 1];
                    x2r = pai[j2] + pai[j3];
                    x2i = pai[j2 + 1] + pai[j3 + 1];
                    x3r = pai[j2] - pai[j3];
                    x3i = pai[j2 + 1] - pai[j3 + 1];
                    pai[j] = x0r + x2r;
                    pai[j + 1] = x0i + x2i;
                    pai[j2] = x0i - x2i;
                    pai[j2 + 1] = x2r - x0r;
                    x0r = x1r + x3i;
                    x0i = x1i - x3r;
                    pai[j1] = wk1r * (x0i + x0r);
                    pai[j1 + 1] = wk1r * (x0i - x0r);
                    x0r = x3i - x1r;
                    x0i = x3r + x1i;
                    pai[j3] = wk1r * (x0r + x0i);
                    pai[j3 + 1] = wk1r * (x0r - x0i);
                }
                k1 = 1;
                ks = -1;
                for (k = (m << 1); k < iWidth ; k += m) {
                    k1++;
                    ks = -ks;
                    wk1r = Wsp[k1 << 1];
                    wk1i = Wsp[(k1 << 1) + 1];
                    wk2r = ks * Wsp[k1];
                    wk2i = Wsp[k1 + ks];
                    wk3r = wk1r - 2 * wk2i * wk1i;
                    wk3i = 2 * wk2i * wk1r - wk1i;
                    for (j = k; j < l + k ; j += 2) {
                        j1 = j + l;
                        j2 = j1 + l;
                        j3 = j2 + l;
                        x0r = pai[j] + pai[j1];
                        x0i = pai[j + 1] + pai[j1 + 1];
                        x1r = pai[j] - pai[j1];
                        x1i = pai[j + 1] - pai[j1 + 1];
                        x2r = pai[j2] + pai[j3];
                        x2i = pai[j2 + 1] + pai[j3 + 1];
                        x3r = pai[j2] - pai[j3];
                        x3i = pai[j2 + 1] - pai[j3 + 1];
                        pai[j] = x0r + x2r;
                        pai[j + 1] = x0i + x2i;
                        x0r -= x2r;
                        x0i -= x2i;
                        pai[j2] = wk2r * x0r + wk2i * x0i;
                        pai[j2 + 1] = wk2r * x0i - wk2i * x0r;
                        x0r = x1r + x3i;
                        x0i = x1i - x3r;
                        pai[j1] = wk1r * x0r + wk1i * x0i;
                        pai[j1 + 1] = wk1r * x0i - wk1i * x0r;
                        x0r = x1r - x3i;
                        x0i = x1i + x3r;
                        pai[j3] = wk3r * x0r + wk3i * x0i;
                        pai[j3 + 1] = wk3r * x0i - wk3i * x0r;
                    }
                }
            }
            l = m;
        }
        if (l < iWidth) {
            for (j = 0; j < l ; j += 2) {
                j1 = j + l;
                x0r = pai[j] - pai[j1];
                x0i = pai[j + 1] - pai[j1 + 1];
                pai[j] += pai[j1];
                pai[j + 1] += pai[j1 + 1];
                pai[j1] = x0r;
                pai[j1 + 1] = x0i;
            }
        }
    }
}



//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void cftfrow(int iHeight, int iWidth, T1_ *pa, T1_ *Wsp)
{
    int		i, j,  k, k1, ks, l, ll, m;
    T1_		wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    T1_		x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    T1_		*paj0, *paj1, *paj2, *paj3;


    l = 1;
    while ((l << 1) < iHeight) {
        m = l << 2;
        ll = l*iWidth;
        for (j = 0; j < l ; j++) {
            paj0 = &pa[j*iWidth];
            paj1 = &paj0[ll];
            paj2 = &paj1[ll];
            paj3 = &paj2[ll];
            for (i = 0; i < iWidth ; i += 2) {
                x0r = paj0[i] + paj1[i];
                x0i = paj0[i + 1] + paj1[i + 1];
                x1r = paj0[i] - paj1[i];
                x1i = paj0[i + 1] - paj1[i + 1];
                x2r = paj2[i] + paj3[i];
                x2i = paj2[i + 1] + paj3[i + 1];
                x3r = paj2[i] - paj3[i];
                x3i = paj2[i + 1] - paj3[i + 1];
                paj0[i] = x0r + x2r;
                paj0[i + 1] = x0i + x2i;
                paj2[i] = x0r - x2r;
                paj2[i + 1] = x0i - x2i;
                paj1[i] = x1r + x3i;
                paj1[i + 1] = x1i - x3r;
                paj3[i] = x1r - x3i;
                paj3[i + 1] = x1i + x3r;
            }
        }
        if (m < iHeight) {
            wk1r = Wsp[2];
            for (j = m; j < l + m ; j++) {
                paj0 = &pa[j*iWidth];
                paj1 = &paj0[ll];
                paj2 = &paj1[ll];
                paj3 = &paj2[ll];
                for (i = 0; i < iWidth ; i += 2) {
                    x0r = paj0[i] + paj1[i];
                    x0i = paj0[i + 1] + paj1[i + 1];
                    x1r = paj0[i] - paj1[i];
                    x1i = paj0[i + 1] - paj1[i + 1];
                    x2r = paj2[i] + paj3[i];
                    x2i = paj2[i + 1] + paj3[i + 1];
                    x3r = paj2[i] - paj3[i];
                    x3i = paj2[i + 1] - paj3[i + 1];
                    paj0[i] = x0r + x2r;
                    paj0[i + 1] = x0i + x2i;
                    paj2[i] = x0i - x2i;
                    paj2[i + 1] = x2r - x0r;
                    x0r = x1r + x3i;
                    x0i = x1i - x3r;
                    paj1[i] = wk1r * (x0i + x0r);
                    paj1[i + 1] = wk1r * (x0i - x0r);
                    x0r = x3i - x1r;
                    x0i = x3r + x1i;
                    paj3[i] = wk1r * (x0r + x0i);
                    paj3[i + 1] = wk1r * (x0r - x0i);
                }
            }
            k1 = 1;
            ks = -1;
            for (k = (m << 1); k < iHeight ; k += m) {
                k1++;
                ks = -ks;
                wk1r = Wsp[k1 << 1];
                wk1i = Wsp[(k1 << 1) + 1];
                wk2r = ks * Wsp[k1];
                wk2i = Wsp[k1 + ks];
                wk3r = wk1r - 2 * wk2i * wk1i;
                wk3i = 2 * wk2i * wk1r - wk1i;
                for (j = k; j < l + k ; j++) {
                    paj0 = &pa[j*iWidth];
                    paj1 = &paj0[ll];
                    paj2 = &paj1[ll];
                    paj3 = &paj2[ll];
                    for (i = 0; i < iWidth ; i += 2) {
                        x0r = paj0[i] + paj1[i];
                        x0i = paj0[i + 1] + paj1[i + 1];
                        x1r = paj0[i] - paj1[i];
                        x1i = paj0[i + 1] - paj1[i + 1];
                        x2r = paj2[i] + paj3[i];
                        x2i = paj2[i + 1] + paj3[i + 1];
                        x3r = paj2[i] - paj3[i];
                        x3i = paj2[i + 1] - paj3[i + 1];
                        paj0[i] = x0r + x2r;
                        paj0[i + 1] = x0i + x2i;
                        x0r -= x2r;
                        x0i -= x2i;
                        paj2[i] = wk2r * x0r + wk2i * x0i;
                        paj2[i + 1] = wk2r * x0i - wk2i * x0r;
                        x0r = x1r + x3i;
                        x0i = x1i - x3r;
                        paj1[i] = wk1r * x0r + wk1i * x0i;
                        paj1[i + 1] = wk1r * x0i - wk1i * x0r;
                        x0r = x1r - x3i;
                        x0i = x1i + x3r;
                        paj3[i] = wk3r * x0r + wk3i * x0i;
                        paj3[i + 1] = wk3r * x0i - wk3i * x0r;
                    }
                }
            }
        }
        l = m;
    }
    if (l < iHeight) {
        for (j = 0; j < l ; j++) {
            paj0 = &pa[j*iWidth];
            paj1 = &pa[(j + l)*iWidth];
            for (i = 0; i < iWidth ; i += 2) {
                x0r = paj0[i] - paj1[i];
                x0i = paj0[i + 1] - paj1[i + 1];
                paj0[i] += paj1[i];
                paj0[i + 1] += paj1[i + 1];
                paj1[i] = x0r;
                paj1[i + 1] = x0i;
            }
        }
    }
}



//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void rftbcol(int iHeight, int iWidth, T1_ *pa, int nc, T1_ *c)
{
    int		i, j, k, kk, ks;
    T1_		wkr, wki, xr, xi, yr, yi;
    T1_		*pai;

    ks = (nc << 2) / iWidth;
    for (i = 0; i < iHeight ; i++) {
        kk = 0;
        pai = &pa[i*iWidth];
        for (k = (iWidth >> 1) - 2; k >= 2; k -= 2) {
            j = iWidth - k;
            kk += ks;
            wkr = (T1_)(0.5f - c[kk]);
            wki = c[nc - kk];
            xr = pai[k] - pai[j];
            xi = pai[k + 1] + pai[j + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            pai[k] -= yr;
            pai[k + 1] -= yi;
            pai[j] += yr;
            pai[j + 1] -= yi;
        }
    }
}


//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void rftfcol(int iHeight, int iWidth, T1_ *pa, int nc, T1_ *c)
{
    int i, j, k, kk, ks;
    T1_ wkr, wki, xr, xi, yr, yi;
    T1_ *pai;

    ks = (nc << 2) / iWidth;
    for (i = 0; i < iHeight ; i++) {
        kk = 0;
        pai = &pa[i*iWidth];
        for (k = (iWidth >> 1) - 2; k >= 2; k -= 2) {
            j = iWidth - k;
            kk += ks;
            wkr = 0.5f - c[kk];
            wki = c[nc - kk];
            xr = pai[k] - pai[j];
            xi = pai[k + 1] + pai[j + 1];
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            pai[k] -= yr;
            pai[k + 1] -= yi;
            pai[j] += yr;
            pai[j + 1] -= yi;
        }
    }
}




/****************************************************************************
 * ONE DIMENSIONAL FFT FUNCTIONS
 ****************************************************************************/
//  ==========================================================================
//  Function:
//  ==========================================================================
template<typename T1_>
void fMIA_FFT1D(int iWidth,  T1_ *pa, int  iSign, int *iWsp, T1_ *Wsp)
{
    int		n, nw, nc;
    T1_		xi;
    T1_		*pai;

    n = iWidth;

    nw = iWsp[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, iWsp, w);
    }
    nc = iWsp[1];
    if (iWidth > (nc << 2)) {
        nc = iWidth >> 2;
        makect(nc, iWsp, Wsp + nw);
    }

    if  (pa == 0)  return;


    if (iSign < 0) {
        cftfrow(1, iWidth, pa, w);
        pai = &pa[0];
        pai[1] = 0.5f * (pai[0] - pai[1]);
        pai[0] -= pai[1];

        if (iWidth > 4) {
            rftfcol(1, iWidth, pa, nc, Wsp + nw);
            bitrv2col(1, iWidth, iWsp + 2, pa);
        }
        cftfcol(1, iWidth, pa, w);
    } else {
        if (iWidth > 4) {
            bitrv2col(1, iWidth, iWsp + 2, pa);
        }
        cftbcol(1, iWidth, pa, w);
        if (iWidth > 4) {
            rftbcol(1, iWidth, pa, nc, Wsp + nw);
        }
        pai = &pa[0];
        xi = pai[0] - pai[1];
        pai[0] += pai[1];
        pai[1] = xi;

        cftbrow(1, iWidth, pa, w);
    }
}


//  ========================================================================================
//  Function:
//  ========================================================================================
template<typename T1_>
void fMIA_FFT1D_AmplSpec(int iWidth,  T1_ *pIn, T1_ *pOut)
{
 /*
    // Copy the whole Spectrum to the output (DC Part in center)
    int		x,cx;
    T1_		re, im,s;
    T1_		*pa1,*po1;


    cx = iWidth/2;

    pa1 = &pIn[0];
    po1 = &pOut[cx];
    for (x = 1; x < cx ; x++) {
        re = pa1[2*x];
        im = pa1[2*x+1];
        s = sqrt(re*re+im*im);
        po1[x] = s;
        po1[-x] = s;
    }

    re = pIn[0];
    pOut[cx] = abs(re);
    re = pIn[1];
    pOut[0] = abs(re);
    */

    // Copy from cell 0 = DC to iWidth/2. Rest of outputarray is
    // left untouched, dont use!!!
    int		x,cx;
     T1_		re, im,s;

     cx = iWidth/2;

     for (x = 1; x < cx ; x++) {
         re = pIn[2*x];         // real part
         im = pIn[2*x+1];       // imaginary part
         s = sqrt(re*re+im*im); // absolute value
         pOut[x] = s;
     }

     re = pIn[0];
     pOut[0] = abs(re);
     re = pIn[1];
     pOut[cx] = abs(re);

}
